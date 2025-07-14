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
    render::window::setTitle("Vertex attribute test");

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 2.0f, 2.0f});

    auto shader = render::Shader::create(
        util::getResourcePath("shaders/entity.vs"),
        util::getResourcePath("shaders/entity.fs")
    );

    // Generate instancing data
    struct instanceData {
        glm::vec4 color;
        glm::vec3 pos;
        float radius;
    };

    std::vector<instanceData> instances;
    for (int i = 0; i < 50000; ++i) {
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
        // float sy = (float)(rand() % 900 + 100) / 100.0f;
        // float sz = (float)(rand() % 900 + 100) / 100.0f;

        // glm::mat4 m{1.0f};
        // m = glm::translate(m, glm::vec3{x, y, z});
        // m = glm::scale(m, glm::vec3{sx, sy, sz});

        instances.push_back(instanceData{.color = color, .pos = glm::vec3{x, y, z}, .radius = sx});
    }

    // Generate mesh vertices
    std::vector<util::GeneralVertex> vertices;
    std::vector<unsigned int> indices;
    util::generateSphereMesh([&](auto v) { vertices.push_back(v); }, indices, 20);
    printf("vertices: %ld | indices: %ld\n", vertices.size(), indices.size());
    
    // Set attributes and data
    render::Mesh<util::GeneralVertex, instanceData> mesh;
    mesh.setAttributes({
        render::VertexAttributeType::Float3, // position
        render::VertexAttributeType::Float3, // normal
    }, {
        render::VertexAttributeType::Float4, // color
        render::VertexAttributeType::Float3, // pos
        render::VertexAttributeType::Float,  // radius
    });
    mesh.setVertexBufferData(vertices.data(), vertices.size());
    mesh.setIndexBufferData(indices);
    mesh.setInstanceBufferData(instances.data(), instances.size());

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

        for (size_t i = 0; i < instances.size(); ++i) {
            glm::vec3 pos = glm::vec3(instances[i].pos);
            pos += glm::vec3{glm::sin(runtime + i * 0.01f), glm::cos(runtime - i * 0.1f), 0.0f};

            float radius = instances[i].radius;

            float t = runtime * 10.0f + i * 0.01f;
            radius = glm::sin(t) * 4.5f + 10.0f;

            instances[i].pos = pos;
            instances[i].radius = radius;
        }
        mesh.setInstanceBufferData(instances.data(), instances.size());

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

        shader->setMat4("model", glm::mat4{1.0f});
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("viewPos", camera.getPosition());
        shader->setVec4("meshColor", glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        mesh.render();

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