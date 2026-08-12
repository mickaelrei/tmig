#include <memory>
#include <stdexcept>

#include "glad/glad.h"

#include "tmig/util/postprocessing.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/mesh.hpp"

namespace tmig::util {

/// @brief Helper struct for storing all screen quad resources
struct ScreenQuadRenderer {
    struct Vert {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    render::ShaderProgram shader;
    render::Mesh<Vert> mesh;
    std::unique_ptr<render::DataBuffer<Vert>> vertexBuffer;
    std::unique_ptr<render::DataBuffer<uint32_t>> indexBuffer;

    ScreenQuadRenderer() {
        // Prepare shader
        if (!shader.compileFromFiles(
            getResourcePath("engine/shaders/screen_quad.vert"),
            getResourcePath("engine/shaders/screen_quad.frag")
        )) {
            throw std::runtime_error{"Failed to compile screen quad shader"};
        }

        // Prepare mesh data
        std::vector<Vert> vertices;
        std::vector<uint32_t> indices;
        generateScreenQuadMesh([&vertices](auto v) {
            vertices.push_back(Vert{
                .pos = v.position,
                .uv = v.uv
            });
        }, indices);

        vertexBuffer = std::make_unique<render::DataBuffer<Vert>>();
        vertexBuffer->setData(vertices);

        indexBuffer = std::make_unique<render::DataBuffer<uint32_t>>();
        indexBuffer->setData(indices);

        // Configure the mesh
        mesh.setAttributes({
            render::VertexAttributeType::FLOAT3, // pos
            render::VertexAttributeType::FLOAT2, // uv
        });
        mesh.setVertexBuffer(vertexBuffer.get());
        mesh.setIndexBuffer(indexBuffer.get());
    }
};

void renderScreenQuadTexture(const render::Texture2D& texture) {
    static ScreenQuadRenderer renderer;

    renderer.shader.use();
    renderer.shader.setTexture("scene", texture, 0);
    glDisable(GL_DEPTH_TEST);
    renderer.mesh.render();
    glEnable(GL_DEPTH_TEST);
}

void renderScreenQuadSplit(const render::Texture2D& left, const render::Texture2D& right) {
    static ScreenQuadRenderer renderer;
    static render::ShaderProgram splitShader;
    static bool splitReady = false;

    if (!splitReady) {
        if (!splitShader.compileFromFiles(
            getResourcePath("engine/shaders/screen_quad.vert"),
            getResourcePath("engine/shaders/screen_quad_split.frag")
        )) {
            throw std::runtime_error{"Failed to compile screen quad split shader"};
        }
        splitReady = true;
    }

    splitShader.use();
    splitShader.setTexture("scene", left, 0);
    splitShader.setTexture("processed", right, 1);
    glDisable(GL_DEPTH_TEST);
    renderer.mesh.render();
    glEnable(GL_DEPTH_TEST);
}

} // namespace tmig::util
