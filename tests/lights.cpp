#include <iostream>
#include <vector>
#include <cmath>

#include "tmig/render/render.hpp"
#include "tmig/render/mesh.hpp"
#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/framebuffer.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/postprocessing/bloom.hpp"
#include "tmig/util/camera_controller.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/time_step.hpp"
#include "tmig/util/postprocessing.hpp"
#include "tmig/util/color.hpp"
#include "tmig/core/light_manager.hpp"
#include "tmig/core/input.hpp"

using namespace tmig;

// Simple vertex struct for this demo
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

// Instance data for the walls
struct InstanceData {
    glm::vec3 color;
    glm::mat4 model;
};

int main() {
    render::init();
    render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

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

    render::ShaderProgram instancedShader;
    if (!instancedShader.compileFromFiles(
        util::getResourcePath("shaders/instanced_lighting.vert"),
        util::getResourcePath("shaders/instanced_lighting.frag")
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

    const uint32_t numMovingLights = 10;
    std::vector<core::PointLightHandle> movingLights;
    for (uint32_t i = 0; i < numMovingLights; ++i) {
        float h = (float)i / (float)numMovingLights;

        movingLights.push_back(lightManager.addPointLight({
            .position = {0.0f, 0.0f, 0.0f},
            .color = util::HSVtoRGB(h, 1.0f, 0.5f),
            .intensity = 1.75f,
            .constant = 1.0f,
            .linear = 0.09f,
            .quadratic = 0.032f
        }));
    }

    auto spotLightHandle = lightManager.addSpotLight({
        .position = camera.getPosition(),
        .direction = camera.getForward(),
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
        render::VertexAttributeType::FLOAT3, // position
        render::VertexAttributeType::FLOAT3, // normal
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
    render::InstancedMesh<Vertex, InstanceData> boxMesh;
    boxMesh.setAttributes(
        {
            render::VertexAttributeType::FLOAT3, // position
            render::VertexAttributeType::FLOAT3  // normal
        },
        {
            render::VertexAttributeType::FLOAT3, // color
            render::VertexAttributeType::MAT4x4  // model
        }
    );

    std::vector<Vertex> boxVertices;
    std::vector<uint32_t> boxIndices;
    util::generateBoxMesh([&](auto v) { boxVertices.push_back({v.position, v.normal}); }, boxIndices);
    
    render::DataBuffer<Vertex> boxVertBuffer;
    render::DataBuffer<uint32_t> boxIdxBuffer;
    boxVertBuffer.setData(boxVertices);
    boxIdxBuffer.setData(boxIndices);
    boxMesh.setVertexBuffer(&boxVertBuffer);
    boxMesh.setIndexBuffer(&boxIdxBuffer);

    // Create 6 instances to form the room
    std::vector<InstanceData> boxInstances;
    const float wallThickness = 1.0f;
    const float roomSize = 50.0f;
    float wallPos = roomSize * 0.5f + wallThickness * 0.5f;

    // Floor
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, -wallPos, 0.0f});
    model = glm::rotate(model, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    boxInstances.push_back({glm::vec3{1.0f}, model});

    // Ceiling
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, wallPos, 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), {1.0f, 0.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    boxInstances.push_back({glm::vec3{1.0f}, model});
    
    // Back Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, 0.0f, -wallPos});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    boxInstances.push_back({glm::vec3{1.0f}, model});

    // Front Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {0.0f, 0.0f, wallPos});
    model = glm::rotate(model, glm::radians(180.0f), {0.0f, 1.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    boxInstances.push_back({glm::vec3{1.0f}, model});

    // Left Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {-wallPos, 0.0f, 0.0f});
    model = glm::rotate(model, glm::radians(90.0f), {0.0f, 1.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    boxInstances.push_back({glm::vec3{1.0f}, model});

    // Right Wall
    model = glm::mat4(1.0f);
    model = glm::translate(model, {wallPos, 0.0f, 0.0f});
    model = glm::rotate(model, glm::radians(-90.0f), {0.0f, 1.0f, 0.0f});
    model = glm::scale(model, {roomSize, roomSize, wallThickness});
    boxInstances.push_back({glm::vec3{1.0f}, model});

    render::DataBuffer<InstanceData> wallInstanceBuffer;
    wallInstanceBuffer.setData(boxInstances);
    boxMesh.setInstanceBuffer(&wallInstanceBuffer);

    // --- Ground Spheres Setup (Instanced) ---
    render::InstancedMesh<Vertex, InstanceData> sphereMesh;
    sphereMesh.setAttributes(
        {
            render::VertexAttributeType::FLOAT3, // position
            render::VertexAttributeType::FLOAT3  // normal
        },
        {
            render::VertexAttributeType::FLOAT3, // color
            render::VertexAttributeType::MAT4x4  // model
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

    // Create instances that will follow the lights
    std::vector<InstanceData> sphereInstances;
    for (uint32_t i = 0; i < numMovingLights; ++i) {
        sphereInstances.push_back({});
    }

    // Create ground instances
    const uint32_t sphereCount = 10;
    const float sphereRadius = roomSize / sphereCount;
    for (uint32_t i = 0; i < sphereCount; ++i) {
        for (uint32_t j = 0; j < sphereCount; ++j) {
            float x = (sphereRadius - roomSize) * 0.5f + (float)i * roomSize / (float)sphereCount;
            float z = (sphereRadius - roomSize) * 0.5f + (float)j * roomSize / (float)sphereCount;

            glm::mat4 model{1.0f};
            model = glm::translate(model, glm::vec3{x, (sphereRadius - roomSize) * 0.5f, z});
            model = glm::scale(model, glm::vec3{sphereRadius});
            sphereInstances.push_back({glm::vec3{1.0f}, model});
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
    render::postprocessing::BloomEffect bloomEffect{{.blurWidth = 600, .blurHeight = 600}};
    bloomEffect.blurIterations = 1;
    bloomEffect.setThreshold(1.0f);
    bloomEffect.setOffsetScale(0.25f);
    bloomEffect.setStrength(0.125f);

    // --- Main Loop ---
    util::TimeStep timeStep;
    util::SmoothFreeFlyCameraController camController;
    bool applyBloom = true;
    while (!render::window::shouldClose()) {
        core::input::update();

        float runtime = render::window::getRuntime();
        if (timeStep.update(runtime)) {
            std::string newTitle = "Lighting test | FPS: " + std::to_string(static_cast<int>(std::round(timeStep.fps())));
            render::window::setTitle(newTitle);
        }

        if (isKeyPressed(core::input::Key::ESCAPE)) {
            render::window::setShouldClose(true);
        }

        if (isKeyPressed(core::input::Key::F)) {
            applyBloom = !applyBloom;
        }

        camController.update(camera, timeStep.dt());

        // --- Animate Lights ---
        const float t = runtime * 0.5f;
        for (uint32_t i = 0; i < numMovingLights; ++i) {
            float angle = t + M_PIf * 2.0f / numMovingLights * i;
            float h = std::fmod((float)i / (float)numMovingLights + t, 1.0f);

            auto pos = glm::vec3{sin(angle) * 8.0f, 0.0f, cos(angle) * 8.0f};
            auto color = util::HSVtoRGB(h, 1.0f, 0.5f);

            movingLights[i].setPosition(pos);
            movingLights[i].setColor(color);

            glm::mat4 model{1.0f};
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3{1.25f});
            sphereInstances[i].model = model;
            sphereInstances[i].color = color * 20.0f;
        }
        sphereInstanceBuffer.setSubset(0, numMovingLights, sphereInstances.data());
        spotLightHandle.setPosition(camera.getPosition());
        spotLightHandle.setDirection(camera.getForward());
        lightManager.update();

        // --- Render Scene ---
        sceneFramebuffer.bind();

        // Update Scene UBO
        auto windowSize = render::window::getSize();
        sceneDataUBO.projection = glm::perspective(glm::radians(camera.fov), (float)windowSize.x / (float)windowSize.y, 0.1f, 100.0f);
        sceneDataUBO.view = camera.getViewMatrix();
        sceneDataUBO.viewPos = camera.getPosition();
        ubo.setData(sceneDataUBO);
        
        // Draw the Room (Instanced)
        instancedShader.use();
        boxMesh.render();

        // Draw the Spheres (Instanced)
        sphereMesh.render();
        
        // Draw the Torus (Non-Instanced)
        shader.use();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(2.0f));
        shader.setMat4("model", model);
        shader.setVec3("objectColor", glm::vec3{1.0f});
        torusMesh.render();

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
    }

    return 0;
}