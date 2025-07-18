#include <chrono>
#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "tmig/render/mesh.hpp"
#include "tmig/render/render.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/window.hpp"
#include "tmig/util/camera.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/shapes.hpp"

using namespace tmig;

bool firstSinceLast = true;
float cameraSpeed = 1000.0f;
float cameraRotationSpeed = 0.3f;

int main() {
    srand(3);

    render::init();
    render::window::setTitle("Vertex attribute test | Instancing OFF");

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 2.0f, 2.0f});

    auto shader = render::Shader::create(
        util::getResourcePath("shaders/non_instanced.vert"),
        util::getResourcePath("shaders/base.frag")
    );

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
    std::vector<util::GeneralVertex> highResVertices;
    std::vector<unsigned int> highResIndices;
    util::generateSphereMesh([&](auto v) { highResVertices.push_back(v); }, highResIndices, 40);
    printf(
        "high-res vertices: %ld | high-res indices: %ld\n",
        highResVertices.size(),
        highResIndices.size()
    );

    // Generate low-resolution sphere mesh
    std::vector<util::GeneralVertex> lowResVertices;
    std::vector<unsigned int> lowResIndices;
    util::generateSphereMesh([&](auto v) { lowResVertices.push_back(v); }, lowResIndices, 3);
    printf(
        "low-res vertices: %ld | low-res indices: %ld\n",
        lowResVertices.size(),
        lowResIndices.size()
    );

    // Create high-res vertex data buffer
    auto highResBuffer = std::make_shared<render::DataBuffer<util::GeneralVertex>>();
    highResBuffer->setData(highResVertices);

    // Create low-res vertex data buffer
    auto lowResBuffer = std::make_shared<render::DataBuffer<util::GeneralVertex>>();
    lowResBuffer->setData(lowResVertices);
    
    // Set mesh attributes
    render::Mesh<util::GeneralVertex> mesh;
    mesh.setAttributes({
        render::VertexAttributeType::Float3, // position
        render::VertexAttributeType::Float3, // normal
    });

    // Start with high res
    mesh.setIndexBufferData(highResIndices);
    mesh.setVertexBuffer(highResBuffer);

    float lastTime = render::window::getRuntime();
    while (!render::window::shouldClose()) {
        // Calculate dt
        float runtime = render::window::getRuntime();
        float dt = runtime - lastTime;
        lastTime = runtime;

        // Close window if ESC was pressed
        if (render::window::getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            render::window::setShouldClose(true);
        }
        if (render::window::getKeyState(GLFW_KEY_E) == GLFW_PRESS) {
            // Change to low-res data on E key
            mesh.setIndexBufferData(lowResIndices);
            mesh.setVertexBuffer(lowResBuffer);
        }
        if (render::window::getKeyState(GLFW_KEY_F) == GLFW_PRESS) {
            // Change to high-res data on F key
            mesh.setIndexBufferData(highResIndices);
            mesh.setVertexBuffer(highResBuffer);
        }

        util::firstPersonCameraMovement(camera, dt, firstSinceLast, cameraSpeed, cameraRotationSpeed);

        // Set projection
        auto view = camera.getViewMatrix();
        auto windowSize = render::window::getSize();
        auto projection = glm::perspective(
            glm::radians(camera.fov),
            (float)windowSize.x / (float)windowSize.y,
            camera.minDist, camera.maxDist
        );

        auto start = std::chrono::high_resolution_clock::now();
        
        render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        render::clearBuffers();

        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("viewPos", camera.getPosition());

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
            model = glm::scale(model, scale);
            instances[i].model = model;

            shader->setVec4("color", instances[i].color);
            shader->setMat4("model", instances[i].model);
            mesh.render();
        }

        render::window::swapBuffers();
        render::window::pollEvents();

        // Time after drawing
        auto end = std::chrono::high_resolution_clock::now();

        // Display time durations
        auto drawDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        printf("FPS: %4.0f | Draw: %6ld\n", 1.0f / dt, drawDuration);
    }

    return 0;
}