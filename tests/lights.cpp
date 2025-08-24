#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "tmig/render/render.hpp"
#include "tmig/render/mesh.hpp"
#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/window.hpp"
#include "tmig/util/camera.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/time_step.hpp"
#include "tmig/core/light_manager.hpp"

using namespace tmig;

// Camera controls
bool firstSinceLast = true;
float cameraSpeed = 10.0f;
float cameraRotationSpeed = 0.3f;

// Simple vertex struct for this demo
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

// Instance data for the walls
struct InstanceData {
    glm::mat4 model;
};

int main() {
    render::init();
    render::setClearColor({0.01f, 0.01f, 0.01f, 1.0f});

    // Setup Camera
    render::Camera camera;
    camera.setPosition({7.0f, 4.0f, 7.0f});
    camera.rotate(-M_PIf * 0.25f, M_PIf * 0.25f, 0.0f);

    // Setup Shaders
    render::ShaderProgram shader;
    if (!shader.compileFromFiles(
        util::getResourcePath("shaders/lighting.vert"),
        util::getResourcePath("shaders/lighting.frag")
    )) {
        std::cerr << "Failed to compile lighting shader\n";
        return 1;
    }

    render::ShaderProgram instancedShader; // For the walls
    if (!instancedShader.compileFromFiles(
        util::getResourcePath("shaders/instanced_lighting.vert"),
        util::getResourcePath("shaders/lighting.frag")
    )) {
        std::cerr << "Failed to compile instanced lighting shader\n";
        return 1;
    }

    // --- Scene and Light Setup ---

    // 1. Create the LightManager and bind its UBO to binding point 1
    core::LightManager lightManager;
    lightManager.bindTo(1);

    // 2. Add lights to the manager and store their handles
    lightManager.addDirectionalLight({
        .direction = {-0.2f, -1.0f, -0.3f},
        .color = glm::vec3{1.0f},
        .intensity = 0.15f,
    });

    auto pointLightHandle = lightManager.addPointLight({
        .position = {0.0f, 0.0f, 0.0f},
        .color = {1.0f, 0.5f, 0.7f},
        .intensity = 1.75f,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    });

    auto spotLightHandle = lightManager.addSpotLight({
        .position = camera.getPosition(),
        .direction = camera.forward(),
        .color = glm::vec3{1.0f},
        .intensity = 2.0f,
        .cutOff = glm::cos(glm::radians(12.5f)),
        .outerCutOff = glm::cos(glm::radians(20.0f)),
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    });

    // --- Mesh Setup ---
    render::Mesh<Vertex> torusMesh;
    torusMesh.setAttributes({
        render::VertexAttributeType::Float3, // position
        render::VertexAttributeType::Float3, // normal
    });

    // Generate Torus
    std::vector<Vertex> torusVertices;
    std::vector<uint32_t> torusIndices;
    render::DataBuffer<Vertex> torusVertBuffer;
    render::DataBuffer<uint32_t> torusIdxBuffer;
    util::generateTorusMesh([&](auto v) { torusVertices.push_back({v.position, v.normal}); }, torusIndices, 50);
    torusVertBuffer.setData(torusVertices);
    torusIdxBuffer.setData(torusIndices);
    torusMesh.setVertexBuffer(&torusVertBuffer);
    torusMesh.setIndexBuffer(&torusIdxBuffer);

    // --- Room Wall Setup (Instanced) ---
    render::InstancedMesh<Vertex, InstanceData> wallMesh;
    wallMesh.setAttributes(
        {
            render::VertexAttributeType::Float3, // position
            render::VertexAttributeType::Float3  // normal
        },
        {
            render::VertexAttributeType::Mat4x4  // model
        }
    );

    std::vector<Vertex> planeVertices;
    std::vector<uint32_t> planeIndices;
    util::generateBoxMesh([&](auto v) { planeVertices.push_back({v.position, v.normal}); }, planeIndices);
    
    render::DataBuffer<Vertex> wallVertBuffer;
    render::DataBuffer<uint32_t> wallIdxBuffer;
    wallVertBuffer.setData(planeVertices);
    wallIdxBuffer.setData(planeIndices);
    wallMesh.setVertexBuffer(&wallVertBuffer);
    wallMesh.setIndexBuffer(&wallIdxBuffer);

    // Create 6 instances to form the room
    std::vector<InstanceData> wallInstances;
    const float wallThickness = 1.0f;
    const float roomSize = 50.0f;
    float wallPos = roomSize * 0.5f + wallThickness * 0.5f;

    // Floor
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, -wallPos, 0.0f});
    model = glm::rotate(model, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    wallInstances.push_back({model});

    // Ceiling
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, wallPos, 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), {1.0f, 0.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    wallInstances.push_back({model});
    
    // Back Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, 0.0f, -wallPos});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    wallInstances.push_back({model});

    // Front Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, 0.0f, wallPos});
    model = glm::rotate(model, glm::radians(180.0f), {0.0f, 1.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    wallInstances.push_back({model});

    // Left Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {-wallPos, 0.0f, 0.0f});
    model = glm::rotate(model, glm::radians(90.0f), {0.0f, 1.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    wallInstances.push_back({model});

    // Right Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {wallPos, 0.0f, 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), {0.0f, 1.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    wallInstances.push_back({model});

    render::DataBuffer<InstanceData> wallInstanceBuffer;
    wallInstanceBuffer.setData(wallInstances);
    wallMesh.setInstanceBuffer(&wallInstanceBuffer);

    // --- Ground Spheres Setup (Instanced) ---
    render::InstancedMesh<Vertex, InstanceData> sphereMesh;
    sphereMesh.setAttributes(
        {
            render::VertexAttributeType::Float3, // position
            render::VertexAttributeType::Float3  // normal
        },
        {
            render::VertexAttributeType::Mat4x4  // model
        }
    );

    std::vector<Vertex> sphereVertices;
    std::vector<uint32_t> sphereIndices;
    util::generateSphereMesh([&](auto v) { sphereVertices.push_back({v.position, v.normal}); }, sphereIndices, 50);
    
    render::DataBuffer<Vertex> sphereVertBuffer;
    render::DataBuffer<uint32_t> sphereIdxBuffer;
    sphereVertBuffer.setData(sphereVertices);
    sphereIdxBuffer.setData(sphereIndices);
    sphereMesh.setVertexBuffer(&sphereVertBuffer);
    sphereMesh.setIndexBuffer(&sphereIdxBuffer);

    // Create 6 instances to form the room
    std::vector<InstanceData> sphereInstances;
    const uint32_t sphereCount = 10;
    const float sphereRadius = roomSize / sphereCount;
    for (uint32_t i = 0; i < sphereCount; ++i) {
        for (uint32_t j = 0; j < sphereCount; ++j) {
            float x = (sphereRadius - roomSize) * 0.5f + (float)i * roomSize / (float)sphereCount;
            float z = (sphereRadius - roomSize) * 0.5f + (float)j * roomSize / (float)sphereCount;

            glm::mat4 model{1.0f};
            model = glm::translate(model, glm::vec3{x, (sphereRadius - roomSize) * 0.5f, z});
            model = glm::scale(model, glm::vec3{sphereRadius});
            sphereInstances.push_back({model});
        }
    }

    render::DataBuffer<InstanceData> sphereInstanceBuffer;
    sphereInstanceBuffer.setData(sphereInstances);
    sphereMesh.setInstanceBuffer(&sphereInstanceBuffer);

    // --- Scene UBO (for camera matrices) ---
    struct SceneData {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 viewPos;
    };
    SceneData sceneDataUBO;
    render::UniformBuffer<SceneData> ubo;
    ubo.bindTo(0);

    // --- Main Loop ---
    util::TimeStep timeStep;
    while (!render::window::shouldClose()) {
        float runtime = render::window::getRuntime();
        if (timeStep.update(runtime)) {
            std::string newTitle = "Lighting test | FPS: " + std::to_string(static_cast<int>(std::round(timeStep.fps())));
            render::window::setTitle(newTitle);
        }

        if (render::window::getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            render::window::setShouldClose(true);
        }

        util::firstPersonCameraMovement(camera, timeStep.dt(), firstSinceLast, cameraSpeed, cameraRotationSpeed);

        // --- Animate Lights ---
        pointLightHandle.setPosition(glm::vec3{sin(runtime * 0.5f) * 12.0f, 0.0f, cos(runtime * 0.5f) * 12.0f});
        pointLightHandle.setColor(glm::vec3{sin(runtime * 0.2f) * 0.5f + 0.5f, sin(runtime * 0.5f) * 0.5f + 0.5f, cos(runtime * 0.6f) * 0.5f + 0.5f});
        spotLightHandle.setPosition(camera.getPosition());
        spotLightHandle.setDirection(camera.forward());
        lightManager.update();

        // --- Render Scene ---
        render::clearBuffers();

        // Update Scene UBO
        auto windowSize = render::window::getSize();
        sceneDataUBO.projection = glm::perspective(glm::radians(camera.fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 100.0f);
        sceneDataUBO.view = camera.getViewMatrix();
        sceneDataUBO.viewPos = camera.getPosition();
        ubo.setData(sceneDataUBO);
        
        // Draw the Room (Instanced)
        instancedShader.use();
        instancedShader.setVec3("objectColor", glm::vec3{1.0f});
        wallMesh.render();

        // Draw the Spheres (Instanced)
        instancedShader.setVec3("objectColor", glm::vec3{1.0f});
        sphereMesh.render();
        
        // Draw the Torus (Non-Instanced)
        shader.use();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(2.0f));
        shader.setMat4("model", model);
        shader.setVec3("objectColor", glm::vec3{1.0f});
        torusMesh.render();

        render::window::swapBuffers();
        render::window::pollEvents();
    }

    return 0;
}