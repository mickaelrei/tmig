#include <iostream>
#include <vector>
#include <chrono>

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
#include "tmig/render/postprocessing/bloom.hpp"
#include "tmig/util/camera.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/postprocessing.hpp"

using namespace tmig;

bool firstSinceLast = true;
float cameraSpeed = 10.0f;
float cameraRotationSpeed = 0.3f;

int main() {
    srand(3);

    render::init();
    render::window::setTitle("Framebuffer test");

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 2.0f, 2.0f});

    render::ShaderProgram meshShader;
    if (!meshShader.compileFromFiles(
        util::getResourcePath("shaders/instanced.vert"),
        util::getResourcePath("shaders/instanced_bloom.frag")
    )) {
        std::cout << "Failed loading instanced shader\n";
        return 1;
    }

    render::Texture2D meshTexture;
    if (!meshTexture.loadFromFile(util::getResourcePath("images/awesomeface.png"))) {
        std::cerr << "Failed to load texture\n";
    }

    meshTexture.setWrapS(render::TextureWrapMode::MIRRORED_REPEAT);
    meshTexture.setWrapT(render::TextureWrapMode::MIRRORED_REPEAT);
    meshTexture.setMinFilter(render::TextureMinFilter::LINEAR_MIPMAP_LINEAR);
    meshTexture.setMagFilter(render::TextureMagFilter::LINEAR);
    meshTexture.generateMipmaps();

    // Generate instancing data
    struct instanceData {
        glm::vec4 color;
        glm::mat4 model;
    };

    std::vector<instanceData> instances;
    for (int i = 0; i < 50000; ++i) {
        // color
        float r = (float)(rand() % 1000) / 1000.0f;
        float g = (float)(rand() % 1000) / 1000.0f;
        float b = (float)(rand() % 1000) / 1000.0f;

        // Small chance of being a glowing instance
        if (rand() % 10 == 1) {
            r *= 15.0f;
            g *= 15.0f;
            b *= 15.0f;
        }
        glm::vec4 color = {r, g, b, 1.0f};

        // pos
        float x = (float)(rand() % 1000 - 500) * 2.0f;
        float y = (float)(rand() % 1000 - 500) * 2.0f;
        float z = (float)(rand() % 1000 - 500) * 2.0f;

        // size
        float sx = (float)(rand() % 900 + 100) / 10.0f;
        float sy = (float)(rand() % 900 + 100) / 10.0f;
        float sz = (float)(rand() % 900 + 100) / 10.0f;

        glm::mat4 m{1.0f};
        m = glm::translate(m, glm::vec3{x, y, z});
        m = glm::scale(m, glm::vec3{sx, sy, sz});

        instances.push_back(instanceData{.color = color, .model = m});
    }

    // Generate mesh vertices
    std::vector<util::GeneralVertex> vertices;
    std::vector<uint32_t> indices;
    util::generateSphereMesh([&](auto v) { vertices.push_back(v); }, indices, 15);

    auto vertexBuffer   = new render::DataBuffer<util::GeneralVertex>;
    auto instanceBuffer = new render::DataBuffer<instanceData>;
    auto indexBuffer    = new render::DataBuffer<uint32_t>;
    vertexBuffer->setData(vertices);
    indexBuffer->setData(indices);
    instanceBuffer->setData(instances);

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

    // Framebuffer to render the main scene to a texture
    render::Framebuffer sceneFramebuffer;
    render::Texture2D sceneDepthTexture;
    render::Texture2D sceneOutputTexture;
    sceneOutputTexture.setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
    sceneOutputTexture.setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);
    auto status = sceneFramebuffer.setup({
        .width = 2000,
        .height = 2000,
        .colorAttachments = {
            {0, render::FramebufferAttachment{
                .texture = &sceneOutputTexture,
                .format = render::TextureFormat::RGBA32F,
            }},
        },
        .depthAttachment = render::FramebufferDepthAttachment{
            .texture = &sceneDepthTexture,
            .format = render::DepthAttachmentFormat::DEPTH24_STENCIL8,
        },
    });
    if (status != render::Framebuffer::Status::COMPLETE) {
        std::cerr << "Framebuffer failed; status: " << status << "\n";
    }

    // Create instance of bloom effect
    render::postprocessing::BloomEffect bloomEffect;
    bloomEffect.setThreshold(1.5f);
    bloomEffect.setOffsetScale(1.5f);
    bloomEffect.setStrength(0.75f);

    float lastTime = render::window::getRuntime();
    render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    bool randomizeMesh = true;
    bool applyBloom = true;
    bool applyTexture = true;
    bool pressingE = false;
    bool pressingF = false;
    bool pressingT = false;
    while (!render::window::shouldClose()) {
        auto start = std::chrono::high_resolution_clock::now();

        // Calculate dt
        float runtime = render::window::getRuntime();
        float dt = runtime - lastTime;
        lastTime = runtime;

        // Close window if ESC was pressed
        if (render::window::getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            render::window::setShouldClose(true);
        }

        if (!pressingE && render::window::getKeyState(GLFW_KEY_E) == GLFW_PRESS) {
            pressingE = true;
            randomizeMesh = !randomizeMesh;
        }
        if (render::window::getKeyState(GLFW_KEY_E) == GLFW_RELEASE) {
            pressingE = false;
        }

        if (!pressingF && render::window::getKeyState(GLFW_KEY_F) == GLFW_PRESS) {
            pressingF = true;
            applyBloom = !applyBloom;
        }
        if (render::window::getKeyState(GLFW_KEY_F) == GLFW_RELEASE) {
            pressingF = false;
        }

        if (!pressingT && render::window::getKeyState(GLFW_KEY_T) == GLFW_PRESS) {
            pressingT = true;
            applyTexture = !applyTexture;
        }
        if (render::window::getKeyState(GLFW_KEY_T) == GLFW_RELEASE) {
            pressingT = false;
        }

        // Update scene
        if (randomizeMesh) {
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
        }

        util::firstPersonCameraMovement(camera, dt, firstSinceLast, cameraSpeed, cameraRotationSpeed);

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
        sceneFramebuffer.bind();
        meshShader.setBool("applyTexture", applyTexture);
        meshShader.setTexture("tex", meshTexture, 0);
        meshShader.use();
        mesh.render();

        if (applyBloom) {
            // Apply bloom effect
            const auto& bloomTexture = bloomEffect.apply(sceneOutputTexture);
            render::Framebuffer::bindDefault(windowSize.x, windowSize.y);
            util::renderScreenQuadTexture(bloomTexture);
        } else {
            // Skip bloom
            render::Framebuffer::bindDefault(windowSize.x, windowSize.y);
            util::renderScreenQuadTexture(sceneOutputTexture);
        }

        render::window::swapBuffers();
        render::window::pollEvents();

        // Display time durations
        auto end = std::chrono::high_resolution_clock::now();
        auto drawDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        printf("FPS: %4.0f | Draw: %6ld\n", 1.0f / dt, drawDuration);
    }

    delete vertexBuffer;
    delete instanceBuffer;
    delete indexBuffer;

    return 0;
}