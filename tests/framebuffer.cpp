#include <iostream>
#include <vector>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "tmig/render/render.hpp"
#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/framebuffer.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/texture2D.hpp"
#include "tmig/util/camera.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/time_step.hpp"

using namespace tmig;

bool firstSinceLast = true;
float cameraSpeed = 100.0f;
float cameraRotationSpeed = 0.3f;

int main() {
    srand(3);

    render::init();

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 2.0f, 2.0f});

    render::ShaderProgram shader;
    if (!shader.compileFromFiles(
        util::getResourcePath("shaders/instanced.vert"),
        util::getResourcePath("shaders/instanced.frag")
    )) {
        std::cout << "Failed loading instanced shader\n";
        return 1;
    }

    render::ShaderProgram postProcessingShader;
    if (!postProcessingShader.compileFromFiles(
        util::getResourcePath("engine/shaders/screen_quad.vert"),
        util::getResourcePath("shaders/post_processing.frag")
    )) {
        std::cout << "Failed loading post_processing shader\n";
        return 1;
    }

    // Creating texture, binding at unit and setting in shader uniform
    render::Texture2D texture;
    if (!texture.loadFromFile(util::getResourcePath("images/awesomeface.png"))) {
        std::cout << "Failed to load texture\n";
        return 1;
    }
    texture.setWrapS(render::TextureWrapMode::MIRRORED_REPEAT);
    texture.setWrapT(render::TextureWrapMode::MIRRORED_REPEAT);
    texture.setMinFilter(render::TextureMinFilter::LINEAR_MIPMAP_LINEAR);
    texture.setMagFilter(render::TextureMagFilter::LINEAR);
    texture.generateMipmaps();
    shader.setTexture("tex", texture, 1);

    // Generate instancing data
    struct instanceData {
        glm::vec4 color;
        glm::mat4 model;
    };

    std::vector<instanceData> instances;
    for (int i = 0; i < 200000; ++i) {
        // color
        float r = (float)(rand() % 1000) / 1000.0f;
        float g = (float)(rand() % 1000) / 1000.0f;
        float b = (float)(rand() % 1000) / 1000.0f;
        glm::vec4 color = {r, g, b, 1.0f};

        // pos
        float x = (float)(rand() % 1000 - 500) * 2.0f;
        float y = (float)(rand() % 1000 - 500) * 2.0f;
        float z = (float)(rand() % 1000 - 500) * 2.0f;

        // size
        float sx = (float)(rand() % 900 + 100) / 100.0f;
        float sy = (float)(rand() % 900 + 100) / 100.0f;
        float sz = (float)(rand() % 900 + 100) / 100.0f;

        glm::mat4 m{1.0f};
        m = glm::translate(m, glm::vec3{x, y, z});
        m = glm::scale(m, glm::vec3{sx, sy, sz});

        instances.push_back(instanceData{.color = color, .model = m});
    }

    // Generate mesh vertices
    std::vector<util::GeneralVertex> vertices;
    std::vector<uint32_t> indices;
    util::generateBoxMesh([&](auto v) { vertices.push_back(v); }, indices);
    printf("vertices: %ld | indices: %ld\n", vertices.size(), indices.size());

    // Create vertex data buffer
    auto vertexBuffer = new render::DataBuffer<util::GeneralVertex>;
    vertexBuffer->setData(vertices);

    // Create instance data buffer
    auto instanceBuffer = new render::DataBuffer<instanceData>;
    instanceBuffer->setData(instances);

    // Create index buffer
    auto indexBuffer = new render::DataBuffer<uint32_t>;
    indexBuffer->setData(indices);

    // Set attributes and data
    render::InstancedMesh<util::GeneralVertex, instanceData> mesh;
    mesh.setAttributes({
        render::VertexAttributeType::Float3, // position
        render::VertexAttributeType::Float3, // normal
        render::VertexAttributeType::Float2, // uv
    }, {
        render::VertexAttributeType::Float4, // color
        render::VertexAttributeType::Mat4x4, // model
    });
    mesh.setInstanceBuffer(instanceBuffer);
    mesh.setIndexBuffer(indexBuffer);
    mesh.setVertexBuffer(vertexBuffer);

    // Scene UBO
    struct sceneData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 viewPos;
    };
    sceneData sceneDataUBO;
    render::UniformBuffer<sceneData> ubo;
    ubo.bindTo(0);

    // Create screen quad for post-processing render
    struct quadVert {
        glm::vec3 pos;
        glm::vec2 uv;
    };
    std::vector<quadVert> quadVertices;
    std::vector<uint32_t> quadIndices;
    util::generateScreenQuadMesh([&](auto v) { quadVertices.push_back(quadVert{
        .pos = v.position,
        .uv = v.uv
    }); }, quadIndices);

    auto screenQuadVertexBuffer = new render::DataBuffer<quadVert>;
    screenQuadVertexBuffer->setData(quadVertices);

    auto screenQuadIndexBuffer = new render::DataBuffer<uint32_t>;
    screenQuadIndexBuffer->setData(quadIndices);

    render::Mesh<quadVert> screenQuadMesh;
    screenQuadMesh.setAttributes({
        render::VertexAttributeType::Float3, // position
        render::VertexAttributeType::Float2, // uv
    });
    screenQuadMesh.setIndexBuffer(screenQuadIndexBuffer);
    screenQuadMesh.setVertexBuffer(screenQuadVertexBuffer);

    // Create framebuffer for post-processing
    auto sceneOutputTexture = new render::Texture2D;
    auto sceneDepthTexture = new render::Texture2D;
    sceneOutputTexture->setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
    sceneOutputTexture->setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);

    render::Framebuffer fb;
    auto status = fb.setup({
        .width = 1200,
        .height = 1200,
        .colorAttachments = {
            {0, render::FramebufferAttachment{
                .texture = sceneOutputTexture,
                .format = render::TextureFormat::RGBA8,
            }},
        },
        .depthAttachment = render::FramebufferDepthAttachment{
            .texture = sceneDepthTexture,
            .format = render::DepthAttachmentFormat::DEPTH24_STENCIL8,
        },
    });
    if (status != render::Framebuffer::Status::COMPLETE) {
        std::cerr << "Framebuffer failed; status: " << status << "\n";
    }

    util::TimeStep timeStep;
    render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    bool pressingE = false;
    int effect = 0;
    while (!render::window::shouldClose()) {
        float runtime = render::window::getRuntime();
        if (timeStep.update(runtime)) {
            std::string newTitle = "Framebuffer test | FPS: " + std::to_string(static_cast<int>(std::round(timeStep.fps())));
            render::window::setTitle(newTitle);
        }

        // Close window if ESC was pressed
        if (render::window::getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            render::window::setShouldClose(true);
        }
        if (!pressingE && render::window::getKeyState(GLFW_KEY_E) == GLFW_PRESS) {
            pressingE = true;
            effect = (effect + 1) % 5;
        }
        if (render::window::getKeyState(GLFW_KEY_E) == GLFW_RELEASE) {
            pressingE = false;
        }

        // ------------------------------------------
        // ------------------------------------------
        // Render scene
        for (size_t i = 0; i < instances.size(); ++i) {
            glm::vec3 pos = glm::vec3(instances[i].model[3]);
            pos += glm::vec3{glm::sin(runtime + i * 0.01f), glm::cos(runtime - i * 0.1f), 0.0f} * .1f;

            // Extract scale from model matrix
            glm::vec3 scale = glm::vec3(
                glm::length(glm::vec3(instances[i].model[0])),
                glm::length(glm::vec3(instances[i].model[1])),
                glm::length(glm::vec3(instances[i].model[2]))
            );

            float t = runtime * 2.0f + i * 0.01f;
            scale.x = glm::sin(t) * 4.5f + 10.0f;
            scale.y = glm::cos(t) * 4.5f + 10.0f;
            scale.z = glm::sin(t + 1.0f) * 4.5f + 10.0f;

            glm::mat4 model{1.0f};
            model = glm::translate(model, pos);
            model = glm::rotate(model, runtime + i * 0.01f, glm::vec3{0.3f, -0.7f, 0.4f});
            model = glm::scale(model, scale);
            instances[i].model = model;
        }
        instanceBuffer->setSubset(0, instanceBuffer->count(), instances.data());

        util::firstPersonCameraMovement(camera, timeStep.dt(), firstSinceLast, cameraSpeed, cameraRotationSpeed);

        // Set scene UBO data
        auto windowSize = render::window::getSize();
        sceneDataUBO.viewPos = camera.getPosition();
        sceneDataUBO.view = camera.getViewMatrix();
        sceneDataUBO.projection = glm::perspective(
            glm::radians(camera.fov),
            (float)windowSize.x / (float)windowSize.y,
            camera.minDist, camera.maxDist
        );
        ubo.setData(sceneDataUBO);

        // Render instanced mesh
        shader.use();
        fb.bind();
        mesh.render();

        // ------------------------------------------
        // ------------------------------------------
        // Post-processing

        // Go back to default framebuffer
        render::Framebuffer::bindDefault(windowSize.x, windowSize.y);

        // Set scene output texture as input to post-processing shader
        postProcessingShader.use();
        postProcessingShader.setInt("effect", effect);
        postProcessingShader.setTexture("scene", *sceneOutputTexture, 0);

        // Disable depth-testing while rendering and enable it back right after
        glDisable(GL_DEPTH_TEST);
        screenQuadMesh.render();
        glEnable(GL_DEPTH_TEST);

        render::window::swapBuffers();
        render::window::pollEvents();
    }

    delete vertexBuffer;
    delete instanceBuffer;
    delete indexBuffer;
    delete screenQuadVertexBuffer;
    delete screenQuadIndexBuffer;
    delete sceneOutputTexture;
    delete sceneDepthTexture;

    return 0;
}