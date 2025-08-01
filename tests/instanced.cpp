#include <chrono>
#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/render.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/texture2D.hpp"
#include "tmig/util/camera.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/shapes.hpp"

using namespace tmig;

bool firstSinceLast = true;
float cameraSpeed = 100.0f;
float cameraRotationSpeed = 0.3f;

int main() {
    srand(3);

    render::init();
    render::window::setTitle("Vertex attribute test | Instancing ON");

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 2.0f, 2.0f});

    auto shader = render::Shader{
        util::getResourcePath("shaders/instanced.vert"),
        util::getResourcePath("shaders/instanced.frag")
    };

    // Creating texture, binding at unit and setting in shader uniform
    render::Texture2D texture;
    if (!texture.loadFromFile(util::getResourcePath("images/container.jpg"))) {
        std::cout << "Failed to load texture\n";
        return 1;
    }
    texture.bind(0);
    shader.setInt("tex", 0);

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
    std::vector<unsigned int> indices;
    util::generateBoxMesh([&](auto v) { vertices.push_back(v); }, indices);
    printf("vertices: %ld | indices: %ld\n", vertices.size(), indices.size());

    // Create vertex data buffer
    auto vertexBuffer = new render::DataBuffer<util::GeneralVertex>;
    vertexBuffer->setData(vertices);

    // Create instance data buffer
    auto instanceBuffer = new render::DataBuffer<instanceData>;
    instanceBuffer->setData(instances);

    // Create index buffer
    auto indexBuffer = new render::DataBuffer<unsigned int>;indexBuffer->setData(indices);

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

        util::firstPersonCameraMovement(camera, dt, firstSinceLast, cameraSpeed, cameraRotationSpeed);

        auto start = std::chrono::high_resolution_clock::now();

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

        render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
        render::clearBuffers();

        shader.setMat4("model", glm::mat4{1.0f});
        shader.setVec4("meshColor", glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        mesh.render();

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