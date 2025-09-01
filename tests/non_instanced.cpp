#include <iostream>

#include "tmig/render/mesh.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/render.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/ui.hpp"
#include "tmig/util/camera_controller.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/time_step.hpp"
#include "tmig/core/input.hpp"

#include "imgui.h"

using namespace tmig;

bool firstSinceLast = true;
float cameraSpeed = 1000.0f;
float cameraRotationSpeed = 0.3f;

int main() {
    srand(3);

    render::init();
    render::ui::init();
    render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 2.0f, 2.0f});

    render::ShaderProgram shader;
    if (!shader.compileFromFiles(
        util::getResourcePath("shaders/non_instanced.vert"),
        util::getResourcePath("shaders/non_instanced.frag")
    )) {
        std::cout << "Failed loading non_instanced shader\n";
        return 1;
    }

    struct myVertex {
        glm::vec3 pos;
        glm::vec3 normal;
    };

    // Generate instancing data
    struct instanceData {
        glm::vec4 color;
        glm::mat4 model;
    };

    std::vector<instanceData> instances;
    for (int i = 0; i < 25000; ++i) {
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

    // Generate high-resolution sphere mesh
    std::vector<myVertex> highResVertices;
    std::vector<uint32_t> highResIndices;
    util::generateSphereMesh([&](auto v) {
        highResVertices.push_back(myVertex{
            .pos = v.position,
            .normal = v.normal,
        });
    }, highResIndices, 40);
    printf(
        "high-res vertices: %ld | high-res indices: %ld\n",
        highResVertices.size(),
        highResIndices.size()
    );

    // Generate low-resolution sphere mesh
    std::vector<myVertex> lowResVertices;
    std::vector<uint32_t> lowResIndices;
    util::generateSphereMesh([&](auto v) {
        lowResVertices.push_back(myVertex{
            .pos = v.position,
            .normal = v.normal,
        });
    }, lowResIndices, 3);
    printf(
        "low-res vertices: %ld | low-res indices: %ld\n",
        lowResVertices.size(),
        lowResIndices.size()
    );

    // Create high-res vertex data buffer using move semantics
    auto highResBuffer = new render::DataBuffer<myVertex>;
    highResBuffer->setData(highResVertices);

    // Create high-res index buffer
    auto highResIndexBuffer = new render::DataBuffer<uint32_t>;
    highResIndexBuffer->setData(highResIndices);

    // Create low-res vertex data buffer
    auto lowResBuffer = new render::DataBuffer<myVertex>;
    lowResBuffer->setData(lowResVertices);

    // Create low-res index buffer
    auto lowResIndexBuffer = new render::DataBuffer<uint32_t>;
    lowResIndexBuffer->setData(lowResIndices);

    // Set mesh attributes
    render::Mesh<myVertex> mesh;
    mesh.setAttributes({
        render::VertexAttributeType::FLOAT3, // position
        render::VertexAttributeType::FLOAT3, // normal
    });

    // Start with high res
    mesh.setIndexBuffer(highResIndexBuffer);
    mesh.setVertexBuffer(highResBuffer);

    // Scene UBO
    struct sceneData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 viewPos;
    };
    sceneData sceneDataUBO;
    render::UniformBuffer<sceneData> ubo;
    ubo.bindTo(0);

    shader.use();
    util::TimeStep timeStep;
    util::SmoothFirstPersonCameraController camController;
    camController.moveSpeed = 100.0f;
    bool lowPoly;
    while (!render::window::shouldClose()) {
        core::input::update();
        render::ui::beginFrame();

        float runtime = render::window::getRuntime();
        if (timeStep.update(runtime)) {
            std::string newTitle = "Vertex attribute test | Instancing OFF | FPS: " + std::to_string(static_cast<int>(std::round(timeStep.fps())));
            render::window::setTitle(newTitle);
        }

        // Close window if ESC was pressed
        if (isKeyPressed(core::input::Key::ESCAPE)) {
            render::window::setShouldClose(true);
        }

        // UI
        ImGui::SetNextWindowSize(ImVec2(100, 60));
        ImGui::Begin("Controls", nullptr);
        if (ImGui::Checkbox("Low-poly", &lowPoly)) {
            if (lowPoly) {
                mesh.setIndexBuffer(lowResIndexBuffer);
                mesh.setVertexBuffer(lowResBuffer);
            } else {
                mesh.setIndexBuffer(highResIndexBuffer);
                mesh.setVertexBuffer(highResBuffer);
            }
        }
        ImGui::End();

        camController.update(camera, timeStep.dt());

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

        render::clearBuffers();

        for (size_t i = 0; i < instances.size(); ++i) {
            glm::vec3 pos = glm::vec3(instances[i].model[3]);
            pos += glm::vec3{glm::sin(runtime + i * 0.01f), glm::cos(runtime - i * 0.1f), 0.0f};

            // Extract scale from model matrix
            glm::vec3 scale = glm::vec3(
                glm::length(glm::vec3(instances[i].model[0])),
                glm::length(glm::vec3(instances[i].model[1])),
                glm::length(glm::vec3(instances[i].model[2]))
            );

            float t = runtime * 1.0f + i * 0.01f;
            scale.x = glm::sin(t) * 4.5f + 10.0f;
            scale.y = glm::cos(t) * 4.5f + 10.0f;
            scale.z = glm::sin(t + 1.0f) * 4.5f + 10.0f;

            glm::mat4 model{1.0f};
            model = glm::translate(model, pos);
            model = glm::rotate(model, runtime + i * 0.01f, glm::vec3{0.3f, -0.7f, 0.4f});
            model = glm::scale(model, scale);
            instances[i].model = model;

            shader.setVec4("color", instances[i].color);
            shader.setMat4("model", instances[i].model);
            mesh.render();
        }

        render::ui::endFrame();
        render::window::swapBuffers();
    }

    delete highResBuffer;
    delete highResIndexBuffer;
    delete lowResBuffer;
    delete lowResIndexBuffer;

    render::ui::terminate();

    return 0;
}