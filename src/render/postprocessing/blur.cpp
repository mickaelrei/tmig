#include <stdexcept>
#include <sstream>

#include "glad/glad.h"

#include "tmig/render/postprocessing/blur.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/shapes.hpp"

namespace tmig::render::postprocessing {

BlurEffect::BlurEffect(const BlurConfig& config) {
    // Setup screen quad
    {
        std::vector<quadVert> vertices;
        std::vector<uint32_t> indices;
        util::generateScreenQuadMesh([&vertices](auto v) { vertices.push_back({
            .pos = v.position,
            .uv = v.uv
        }); }, indices);

        _vertBuffer = new DataBuffer<quadVert>;
        _vertBuffer->setData(vertices);

        _indexBuffer = new DataBuffer<uint32_t>;
        _indexBuffer->setData(indices);

        _screenQuad.setAttributes({
            render::VertexAttributeType::FLOAT3,
            render::VertexAttributeType::FLOAT2,
        });
        _screenQuad.setIndexBuffer(_indexBuffer);
        _screenQuad.setVertexBuffer(_vertBuffer);
    }

    // Setup shaders
    {
        if (!blurShader.compileFromFiles(
            util::getResourcePath("engine/shaders/screen_quad.vert"),
            util::getResourcePath("engine/shaders/blur.frag")
        )) {
            throw std::runtime_error{"[render::postprocessing::BlurEffect] Failed loading blur shader"};
        }
    }

    // Setup ping-pong blur framebuffers
    {
        for (int i = 0; i < 2; ++i) {
            blurTextures[i].setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
            blurTextures[i].setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);
            auto status = blurFramebuffers[i].setup({
                .width = config.width,
                .height = config.height,
                .colorAttachments = {
                    {0, render::FramebufferAttachment{
                        .texture = &blurTextures[i],
                        .format = render::TextureFormat::RGBA16F,
                    }},
                },
            });
            if (status != render::Framebuffer::Status::COMPLETE) {
                std::stringstream ss;
                ss << "[render::postprocessing::BlurEffect] Failed setting up blur framebuffer: " << status;
                throw std::runtime_error{ss.str()};
            }
        }
    }
    
    setOffsetScale(offsetScale);
}

BlurEffect::~BlurEffect() {
    delete _vertBuffer;
    delete _indexBuffer;
}

void BlurEffect::setOffsetScale(float _offsetScale) {
    offsetScale = _offsetScale;
    blurShader.setFloat("offsetScale", offsetScale);
}

const Texture2D& BlurEffect::apply(const Texture2D& input, const PostProcessContext& ctx) {
    (void)ctx;

    bool horizontal = true;
    blurShader.use();

    // The first pass blurs the original input texture
    blurFramebuffers[horizontal].bind({ .clearColor = false, .clearStencil = false, .clearDepth = false });
    blurShader.setInt("horizontal", horizontal);
    blurShader.setTexture("image", input, 0);
    glDisable(GL_DEPTH_TEST);
    _screenQuad.render();
    horizontal = !horizontal;

    // Subsequent passes ping-pong between the internal blur textures
    for (uint32_t i = 1; i < blurIterations * 2U; i++) {
        blurFramebuffers[horizontal].bind({ .clearColor = false, .clearStencil = false, .clearDepth = false });
        blurShader.setInt("horizontal", horizontal);
        blurShader.setTexture("image", blurTextures[!horizontal], 0);
        _screenQuad.render();
        horizontal = !horizontal;
    }

    glEnable(GL_DEPTH_TEST);
    
    // The final result is in the last texture that was rendered to.
    // Since `horizontal` is flipped at the end of the loop, the result is in `blurTextures[!horizontal]`.
    return blurTextures[!horizontal];
}

} // namespace tmig::render::postprocessing