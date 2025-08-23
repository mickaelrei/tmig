#include <iostream>

#include "glad/glad.h"

#include "tmig/render/postprocessing/bloom.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"

namespace tmig::render::postprocessing {

BloomEffect::BloomEffect(const BloomConfig& config) {
    // Setup screen quad
    {
        std::vector<quadVert> vertices;
        std::vector<uint32_t> indices;
        util::generateScreenQuadMesh([&vertices](auto v) { vertices.push_back(quadVert{
            .pos = v.position,
            .uv = v.uv
        }); }, indices);

        vertBuffer = new render::DataBuffer<quadVert>;
        vertBuffer->setData(vertices);

        indexBuffer = new render::DataBuffer<uint32_t>;
        indexBuffer->setData(indices);

        screenQuad.setAttributes({
            render::VertexAttributeType::Float3,
            render::VertexAttributeType::Float2,
        });
        screenQuad.setIndexBuffer(indexBuffer);
        screenQuad.setVertexBuffer(vertBuffer);
    }

    // Setup shaders
    {
        if (!brightPassShader.compileFromFiles(
            util::getResourcePath("engine/shaders/screen_quad.vert"),
            util::getResourcePath("engine/shaders/bloom_bright_pass.frag")
        )) {
            throw std::runtime_error{
                "[render::postprocessing::BloomEffect] Failed loading bright_pass shader"
            };
        }

        if (!blurShader.compileFromFiles(
            util::getResourcePath("engine/shaders/screen_quad.vert"),
            util::getResourcePath("engine/shaders/blur.frag")
        )) {
            throw std::runtime_error{
                "[render::postprocessing::BloomEffect] Failed loading blur shader"
            };
        }

        if (!outputShader.compileFromFiles(
            util::getResourcePath("engine/shaders/screen_quad.vert"),
            util::getResourcePath("engine/shaders/bloom_output.frag")
        )) {
            throw std::runtime_error{
                "[render::postprocessing::BloomEffect] Failed loading bloom_output shader"
            };
        }
    }

    // Setup bright-pass part
    {
        brightPassTexture.setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
        brightPassTexture.setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);
        auto status = brightPassFramebuffer.setup({
            .width = config.brightPassWidth,
            .height = config.brightPassheight,
            .colorAttachments = {
                {0, render::FramebufferAttachment{
                    .texture = &brightPassTexture,
                    .format = render::TextureFormat::RGBA32F,
                }},
            },
        });
        if (status != render::Framebuffer::Status::COMPLETE) {
            std::stringstream ss;
            ss << "[render::postprocessing::BloomEffect] Failed setting up bright_pass framebuffer: " << status;
            throw std::runtime_error{ss.str()};
        }
    }

    // Setup ping-pong blur framebuffers
    {
        for (int i = 0; i < 2; ++i) {
            blurTextures[i].setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
            blurTextures[i].setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);
            auto status = blurFramebuffers[i].setup({
                .width = config.blurWidth,
                .height = config.blurheight,
                .colorAttachments = {
                    {0, render::FramebufferAttachment{
                        .texture = &blurTextures[i],
                        .format = render::TextureFormat::RGBA16F,
                    }},
                },
            });
            if (status != render::Framebuffer::Status::COMPLETE) {
                std::stringstream ss;
                ss << "[render::postprocessing::BloomEffect] Failed setting up blur framebuffer: " << status;
                throw std::runtime_error{ss.str()};
            }
        }
    }

    // Setup output framebuffer
    {
        auto status = outputFramebuffer.setup({
            .width = config.outputWidth,
            .height = config.outputheight,
            .colorAttachments = {
                {0, render::FramebufferAttachment{
                    .texture = &outputTexture,
                    .format = render::TextureFormat::RGBA8,
                }},
            },
        });
        if (status != render::Framebuffer::Status::COMPLETE) {
            std::stringstream ss;
            ss << "[render::postprocessing::BloomEffect] Failed setting up output framebuffer: " << status;
            throw std::runtime_error{ss.str()};
        }
    }

    setThreshold(threshold);
    setOffsetScale(offsetScale);
    setStrength(strength);
}

BloomEffect::~BloomEffect() {
    delete vertBuffer;
    delete indexBuffer;
}

void BloomEffect::setThreshold(float _threshold) {
    threshold = _threshold;
    brightPassShader.setFloat("threshold", threshold);
}

void BloomEffect::setOffsetScale(float _offsetScale) {
    offsetScale = _offsetScale;
    blurShader.setFloat("offsetScale", offsetScale);
}

void BloomEffect::setStrength(float _strength) {
    strength = _strength;
    outputShader.setFloat("strength", strength);
}

const Texture2D& BloomEffect::apply(const Texture2D& input) {
    // Bright pass (get excess light in a separate texture)
    brightPassFramebuffer.bind();
    brightPassShader.use();
    brightPassShader.setTexture("scene", input, 0);
    glDisable(GL_DEPTH_TEST);
    screenQuad.render();
    glEnable(GL_DEPTH_TEST);

    // Blur bright areas with ping-pong framebuffers
    bool horizontal = true;
    blurShader.use();
    for (uint32_t i = 0; i < blurIterations * 2U; i++) {
        blurFramebuffers[horizontal].bind({
            .clearColor = false,
            .clearStencil = false,
            .clearDepth = false,
        });
        blurShader.setInt("horizontal", horizontal);
        blurShader.setTexture("image", i == 0 ? brightPassTexture : blurTextures[!horizontal], 0);
        screenQuad.render();
        horizontal = !horizontal;
    }

    // Final output
    outputFramebuffer.bind();
    outputShader.use();
    outputShader.setTexture("scene", input, 0);
    outputShader.setTexture("bloomBlur", blurTextures[1], 1);
    
    glDisable(GL_DEPTH_TEST);
    screenQuad.render();
    glEnable(GL_DEPTH_TEST);
    
    return outputTexture;
}

} // namespace tmig::render::postprocessing
