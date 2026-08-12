#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include "tmig/render/render.hpp"
#include "tmig/render/mesh.hpp"
#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/framebuffer.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/postprocessing/bloom.hpp"
#include "tmig/render/ui.hpp"
#include "tmig/util/camera_controller.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/time_step.hpp"
#include "tmig/util/postprocessing.hpp"
#include "tmig/util/color.hpp"
#include "tmig/core/light_manager.hpp"
#include "tmig/core/input.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"

using namespace tmig;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
};

struct InstanceData {
    glm::vec4 color;
    glm::mat4 model;
};

struct SceneData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 viewPos;
};

static glm::mat4 trs(const glm::vec3& pos, const glm::vec3& scale, float angle = 0.0f, glm::vec3 axis = {0.0f, 1.0f, 0.0f}) {
    glm::mat4 m{1.0f};
    m = glm::translate(m, pos);
    if (angle != 0.0f) {
        m = glm::rotate(m, angle, axis);
    }
    m = glm::scale(m, scale);
    return m;
}

int main() {
    render::init();
    render::ui::init();
    render::window::setSize({1280, 720});
    render::setClearColor(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

    render::Camera camera;
    camera.setPosition({10.0f, 5.0f, 10.0f});
    camera.lookAt({0.0f, 0.5f, 0.0f});

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

    core::LightManager lightManager;
    lightManager.bindTo(1);

    auto directionalHandle = lightManager.addDirectionalLight({
        .direction = {-0.25f, -1.0f, -0.15f},
        .color = glm::vec3{0.85f, 0.9f, 1.0f},
        .intensity = 0.12f,
    });

    constexpr int kMaxMovingLights = 12;
    int numMovingLights = 8;
    std::vector<core::PointLightHandle> movingLights;
    movingLights.reserve(static_cast<size_t>(kMaxMovingLights));
    for (int i = 0; i < kMaxMovingLights; ++i) {
        movingLights.push_back(lightManager.addPointLight({
            .position = {0.0f, 0.0f, 0.0f},
            .color = util::HSVtoRGB(i / float(kMaxMovingLights), 1.0f, 1.0f),
            .intensity = i < numMovingLights ? 2.2f : 0.0f,
            .constant = 1.0f,
            .linear = 0.09f,
            .quadratic = 0.032f
        }));
    }

    auto spotLightHandle = lightManager.addSpotLight({
        .position = camera.getPosition(),
        .direction = camera.getForward(),
        .color = glm::vec3{1.0f},
        .intensity = 2.4f,
        .cutOff = glm::cos(glm::radians(12.5f)),
        .outerCutOff = glm::cos(glm::radians(20.0f)),
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    });

    render::Mesh<Vertex> torusMesh;
    torusMesh.setAttributes({
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT3,
    });

    std::vector<Vertex> torusVertices;
    std::vector<uint32_t> torusIndices;
    render::DataBuffer<Vertex> torusVertBuffer;
    render::DataBuffer<uint32_t> torusIdxBuffer;
    util::generateTorusMesh([&](auto v) {
        torusVertices.push_back({v.position, v.normal});
    }, torusIndices, 64);
    torusVertBuffer.setData(torusVertices);
    torusIdxBuffer.setData(torusIndices);
    torusMesh.setVertexBuffer(&torusVertBuffer);
    torusMesh.setIndexBuffer(&torusIdxBuffer);

    render::InstancedMesh<Vertex, InstanceData> boxMesh;
    boxMesh.setAttributes(
        {
            render::VertexAttributeType::FLOAT3,
            render::VertexAttributeType::FLOAT3
        },
        {
            render::VertexAttributeType::FLOAT4,
            render::VertexAttributeType::MAT4x4
        }
    );

    std::vector<Vertex> boxVertices;
    std::vector<uint32_t> boxIndices;
    util::generateBoxMesh([&](auto v) {
        boxVertices.push_back({v.position, v.normal});
    }, boxIndices);

    render::DataBuffer<Vertex> boxVertBuffer;
    render::DataBuffer<uint32_t> boxIdxBuffer;
    boxVertBuffer.setData(boxVertices);
    boxIdxBuffer.setData(boxIndices);
    boxMesh.setVertexBuffer(&boxVertBuffer);
    boxMesh.setIndexBuffer(&boxIdxBuffer);

    std::vector<InstanceData> boxInstances;
    const float wallThickness = 1.0f;
    const float roomSize = 36.0f;
    float wallPos = roomSize * 0.5f + wallThickness * 0.5f;

    auto addWall = [&](const glm::vec3& pos, const glm::vec3& scale, float angle, const glm::vec3& axis, const glm::vec3& color) {
        boxInstances.push_back({glm::vec4{color, 1.0f}, trs(pos, scale, angle, axis)});
    };

    // Floor, ceiling, four walls — slightly different tints so bounce color reads clearly
    addWall({0.0f, -wallPos, 0.0f}, {roomSize, roomSize, wallThickness}, glm::radians(90.0f), {1, 0, 0}, {0.82f, 0.78f, 0.72f});
    addWall({0.0f,  wallPos, 0.0f}, {roomSize, roomSize, wallThickness}, glm::radians(-90.0f), {1, 0, 0}, {0.55f, 0.58f, 0.65f});
    addWall({0.0f, 0.0f, -wallPos}, {roomSize, roomSize, wallThickness}, 0.0f, {0, 1, 0}, {0.75f, 0.75f, 0.78f});
    addWall({0.0f, 0.0f,  wallPos}, {roomSize, roomSize, wallThickness}, glm::radians(180.0f), {0, 1, 0}, {0.75f, 0.75f, 0.78f});
    addWall({-wallPos, 0.0f, 0.0f}, {roomSize, roomSize, wallThickness}, glm::radians(90.0f), {0, 1, 0}, {0.72f, 0.74f, 0.78f});
    addWall({ wallPos, 0.0f, 0.0f}, {roomSize, roomSize, wallThickness}, glm::radians(-90.0f), {0, 1, 0}, {0.72f, 0.74f, 0.78f});

    // Pillars to catch colored specular
    for (int i = 0; i < 4; ++i) {
        float a = i * (0.5f * 3.14159265f) + 0.25f * 3.14159265f;
        addWall(
            {std::cos(a) * 11.0f, -wallPos + 6.0f, std::sin(a) * 11.0f},
            {1.4f, 12.0f, 1.4f},
            0.0f, {0, 1, 0},
            {0.9f, 0.9f, 0.92f}
        );
    }

    render::DataBuffer<InstanceData> wallInstanceBuffer;
    wallInstanceBuffer.setData(boxInstances);
    boxMesh.setInstanceBuffer(&wallInstanceBuffer);

    render::InstancedMesh<Vertex, InstanceData> sphereMesh;
    sphereMesh.setAttributes(
        {
            render::VertexAttributeType::FLOAT3,
            render::VertexAttributeType::FLOAT3
        },
        {
            render::VertexAttributeType::FLOAT4,
            render::VertexAttributeType::MAT4x4
        }
    );

    std::vector<Vertex> sphereVertices;
    std::vector<uint32_t> sphereIndices;
    util::generateSphereMesh([&](auto v) {
        sphereVertices.push_back({v.position, v.normal});
    }, sphereIndices, 32);

    render::DataBuffer<Vertex> sphereVertBuffer;
    render::DataBuffer<uint32_t> sphereIdxBuffer;
    sphereVertBuffer.setData(sphereVertices);
    sphereIdxBuffer.setData(sphereIndices);
    sphereMesh.setVertexBuffer(&sphereVertBuffer);
    sphereMesh.setIndexBuffer(&sphereIdxBuffer);

    std::vector<InstanceData> sphereInstances(kMaxMovingLights);

    const uint32_t sphereCount = 8;
    const float sphereRadius = roomSize / sphereCount;
    for (uint32_t i = 0; i < sphereCount; ++i) {
        for (uint32_t j = 0; j < sphereCount; ++j) {
            float x = (sphereRadius - roomSize) * 0.5f + (float)i * roomSize / (float)sphereCount;
            float z = (sphereRadius - roomSize) * 0.5f + (float)j * roomSize / (float)sphereCount;
            sphereInstances.push_back({
                glm::vec4{0.92f, 0.92f, 0.95f, 1.0f},
                trs({x, (sphereRadius - roomSize) * 0.5f + 0.05f, z}, glm::vec3{sphereRadius * 0.85f})
            });
        }
    }

    render::DataBuffer<InstanceData> sphereInstanceBuffer;
    sphereInstanceBuffer.setData(sphereInstances);
    sphereMesh.setInstanceBuffer(&sphereInstanceBuffer);

    SceneData sceneDataUBO;
    render::UniformBuffer<SceneData> ubo;
    ubo.bindTo(0);

    render::Framebuffer sceneFramebuffer;
    render::Texture2D sceneDepthTexture;
    render::Texture2D sceneOutputTexture;
    sceneOutputTexture.setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
    sceneOutputTexture.setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);
    auto status = sceneFramebuffer.setup({
        .width = 1920,
        .height = 1080,
        .colorAttachments = {
            {0, render::FramebufferAttachment{
                .texture = &sceneOutputTexture,
                .format = render::TextureFormat::RGBA16F,
            }},
        },
        .depthAttachment = render::FramebufferDepthAttachment{
            .texture = &sceneDepthTexture,
            .format = render::DepthAttachmentFormat::DEPTH24_STENCIL8,
        },
    });
    if (status != render::Framebuffer::Status::COMPLETE) {
        std::cerr << "Framebuffer failed; status: " << status << "\n";
        return 1;
    }

    render::postprocessing::BloomEffect bloomEffect{{
        .brightPassWidth = 1920,
        .brightPassHeight = 1080,
        .blurWidth = 1600,
        .blurHeight = 900,
        .outputWidth = 1920,
        .outputHeight = 1080,
    }};
    bloomEffect.setThreshold(1.0f);
    bloomEffect.setOffsetScale(0.35f);
    bloomEffect.setStrength(0.2f);

    bool applyBloom = true;
    bool flashlight = true;
    bool animate = true;
    float directionalIntensity = 0.12f;
    float pointIntensity = 2.2f;
    float spotIntensity = 2.4f;
    float orbitRadius = 7.5f;
    float orbitSpeed = 0.45f;
    float specularStrength = 0.65f;
    int shininess = 64;
    glm::ivec2 lastSize{1280, 720};

    util::TimeStep timeStep;
    util::SmoothOrbitalCameraController camController;
    camController.target = {0.0f, 0.5f, 0.0f};
    camController.radius = 14.0f;
    camController.moveSpeed = 8.0f;

    while (!render::window::shouldClose()) {
        core::input::update();
        render::ui::beginFrame();

        float runtime = static_cast<float>(render::window::getRuntime());
        if (timeStep.update(runtime)) {
            render::window::setTitle(
                std::string{"Lights | FPS: "} +
                std::to_string(static_cast<int>(std::round(timeStep.fps())))
            );
        }

        if (isKeyDown(core::input::Key::ESCAPE)) {
            render::window::setShouldClose(true);
        }

        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10));
        ImGui::SetNextWindowSize(ImVec2(320, 340));
        ImGui::Begin("Lights", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::TextWrapped(
            "Closed room lit by orbiting point lights, a dim directional fill, "
            "and a camera flashlight (spot). LightManager uploads them through a UBO."
        );
        ImGui::Separator();
        ImGui::Checkbox("Bloom", &applyBloom);
        ImGui::Checkbox("Flashlight", &flashlight);
        ImGui::Checkbox("Animate lights", &animate);
        ImGui::SliderInt("Point lights", &numMovingLights, 1, kMaxMovingLights);
        ImGui::SliderFloat("Point intensity", &pointIntensity, 0.0f, 6.0f);
        ImGui::SliderFloat("Orbit radius", &orbitRadius, 2.0f, 14.0f);
        ImGui::SliderFloat("Orbit speed", &orbitSpeed, 0.0f, 2.0f);
        ImGui::SliderFloat("Directional", &directionalIntensity, 0.0f, 1.5f);
        ImGui::SliderFloat("Spot intensity", &spotIntensity, 0.0f, 8.0f);
        ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 2.0f);
        ImGui::SliderInt("Shininess", &shininess, 2, 256);
        ImGui::Text("Right-drag to orbit, W/S to zoom");
        ImGui::End();

        auto windowSize = render::window::getSize();
        if (windowSize != lastSize && windowSize.x > 0 && windowSize.y > 0) {
            sceneFramebuffer.resize(static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y));
            lastSize = windowSize;
        }

        camController.update(camera, timeStep.dt());

        float t = animate ? runtime * orbitSpeed : 0.0f;
        for (int i = 0; i < kMaxMovingLights; ++i) {
            float angle = t + 6.2831853f / kMaxMovingLights * i;
            float h = std::fmod(i / float(kMaxMovingLights) + (animate ? runtime * 0.05f : 0.0f), 1.0f);
            auto pos = glm::vec3{std::sin(angle) * orbitRadius, 0.35f + std::sin(angle * 2.0f) * 0.4f, std::cos(angle) * orbitRadius};
            auto color = util::HSVtoRGB(h, 1.0f, 1.0f);
            bool active = i < numMovingLights;

            movingLights[i].setPosition(pos);
            movingLights[i].setColor(color);
            movingLights[i].setIntensity(active ? pointIntensity : 0.0f);

            sphereInstances[i].model = trs(pos, glm::vec3{active ? 0.45f : 0.01f});
            sphereInstances[i].color = glm::vec4{color * (active ? 12.0f : 0.0f), 1.0f};
        }
        sphereInstanceBuffer.setSubset(0, static_cast<size_t>(kMaxMovingLights), sphereInstances.data());

        directionalHandle.setIntensity(directionalIntensity);
        spotLightHandle.setPosition(camera.getPosition());
        spotLightHandle.setDirection(camera.getForward());
        spotLightHandle.setIntensity(flashlight ? spotIntensity : 0.0f);
        lightManager.update();

        sceneFramebuffer.bind();

        sceneDataUBO.projection = glm::perspective(
            glm::radians(camera.fov),
            (float)windowSize.x / (float)windowSize.y,
            0.1f, 200.0f
        );
        sceneDataUBO.view = camera.getViewMatrix();
        sceneDataUBO.viewPos = camera.getPosition();
        ubo.setData(sceneDataUBO);

        instancedShader.use();
        instancedShader.setFloat("specularStrength", specularStrength);
        instancedShader.setInt("shininess", shininess);
        boxMesh.render();
        sphereMesh.render();

        shader.use();
        shader.setFloat("specularStrength", specularStrength * 1.4f);
        shader.setInt("shininess", shininess * 2);
        glm::mat4 model{1.0f};
        model = glm::rotate(model, animate ? runtime * 0.35f : 0.0f, glm::vec3{0.2f, 1.0f, 0.1f});
        model = glm::scale(model, glm::vec3{2.4f});
        shader.setMat4("model", model);
        shader.setVec3("objectColor", glm::vec3{0.95f, 0.92f, 0.88f});
        torusMesh.render();

        if (applyBloom) {
            const auto& bloomTexture = bloomEffect.apply(sceneOutputTexture);
            render::Framebuffer::bindDefault(windowSize.x, windowSize.y);
            util::renderScreenQuadTexture(bloomTexture);
        } else {
            render::Framebuffer::bindDefault(windowSize.x, windowSize.y);
            util::renderScreenQuadTexture(sceneOutputTexture);
        }

        render::ui::endFrame();
        render::window::swapBuffers();
    }

    render::ui::terminate();
    return 0;
}
