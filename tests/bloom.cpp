#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include "tmig/render/render.hpp"
#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/framebuffer.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/texture2D.hpp"
#include "tmig/render/postprocessing/bloom.hpp"
#include "tmig/render/ui.hpp"
#include "tmig/util/camera_controller.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/postprocessing.hpp"
#include "tmig/util/time_step.hpp"
#include "tmig/util/color.hpp"
#include "tmig/core/input.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "glad/glad.h"

using namespace tmig;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
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
    camera.maxDist = 500.0f;
    camera.setPosition(glm::vec3{0.0f, 6.0f, 18.0f});
    camera.lookAt({0.0f, 1.5f, 0.0f});

    render::ShaderProgram meshShader;
    if (!meshShader.compileFromFiles(
        util::getResourcePath("shaders/instanced.vert"),
        util::getResourcePath("shaders/instanced_bloom.frag")
    )) {
        std::cerr << "Failed loading bloom instanced shader\n";
        return 1;
    }

    // Dark non-emissive floor tiles + pillars, plus HDR neon orbs
    std::vector<InstanceData> boxInstances;
    constexpr int grid = 11;
    constexpr float tile = 4.0f;
    for (int z = 0; z < grid; ++z) {
        for (int x = 0; x < grid; ++x) {
            float wx = (x - grid * 0.5f + 0.5f) * tile;
            float wz = (z - grid * 0.5f + 0.5f) * tile;
            bool dark = ((x + z) % 2) == 0;
            glm::vec3 color = dark ? glm::vec3{0.12f} : glm::vec3{0.22f};
            boxInstances.push_back({
                glm::vec4{color, 1.0f},
                trs({wx, -1.5f, wz}, {tile * 0.95f, 0.4f, tile * 0.95f})
            });
        }
    }

    // Dim pillars around the plaza so bloom can bleed onto them
    for (int i = 0; i < 8; ++i) {
        float a = i * (2.0f * 3.14159265f / 8.0f);
        boxInstances.push_back({
            glm::vec4{0.05f, 0.05f, 0.07f, 1.0f},
            trs({std::cos(a) * 16.0f, 2.0f, std::sin(a) * 16.0f}, {1.2f, 8.0f, 1.2f})
        });
    }

    constexpr int kOrbs = 12;
    std::vector<InstanceData> orbInstances(kOrbs);

    render::DataBuffer<Vertex> boxVbo;
    render::DataBuffer<uint32_t> boxIbo;
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        util::generateBoxMesh([&](auto v) {
            vertices.push_back(Vertex{v.position, v.normal, v.uv});
        }, indices);
        boxVbo.setData(vertices);
        boxIbo.setData(indices);
    }

    render::DataBuffer<Vertex> sphereVbo;
    render::DataBuffer<uint32_t> sphereIbo;
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        util::generateSphereMesh([&](auto v) {
            vertices.push_back(Vertex{v.position, v.normal, v.uv});
        }, indices, 24);
        sphereVbo.setData(vertices);
        sphereIbo.setData(indices);
    }

    render::DataBuffer<Vertex> torusVbo;
    render::DataBuffer<uint32_t> torusIbo;
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        util::generateTorusMesh([&](auto v) {
            vertices.push_back(Vertex{v.position, v.normal, v.uv});
        }, indices, 40);
        torusVbo.setData(vertices);
        torusIbo.setData(indices);
    }

    render::DataBuffer<InstanceData> boxInstanceBuffer;
    boxInstanceBuffer.setData(boxInstances);

    render::DataBuffer<InstanceData> orbInstanceBuffer;
    orbInstanceBuffer.setData(orbInstances);

    InstanceData torusInstance{
        glm::vec4{1.2f, 6.0f, 16.0f, 1.0f},
        trs({0.0f, 2.0f, 0.0f}, {4.0f, 4.0f, 4.0f})
    };
    render::DataBuffer<InstanceData> torusInstanceBuffer;
    torusInstanceBuffer.setData(&torusInstance, 1);

    auto configure = [](render::InstancedMesh<Vertex, InstanceData>& mesh,
                        render::DataBuffer<Vertex>* vbo,
                        render::DataBuffer<uint32_t>* ibo,
                        render::DataBuffer<InstanceData>* instances) {
        mesh.setAttributes({
            render::VertexAttributeType::FLOAT3,
            render::VertexAttributeType::FLOAT3,
            render::VertexAttributeType::FLOAT2,
        }, {
            render::VertexAttributeType::FLOAT4,
            render::VertexAttributeType::MAT4x4,
        });
        mesh.setVertexBuffer(vbo);
        mesh.setIndexBuffer(ibo);
        mesh.setInstanceBuffer(instances);
    };

    render::InstancedMesh<Vertex, InstanceData> boxMesh;
    render::InstancedMesh<Vertex, InstanceData> orbMesh;
    render::InstancedMesh<Vertex, InstanceData> torusMesh;
    configure(boxMesh, &boxVbo, &boxIbo, &boxInstanceBuffer);
    configure(orbMesh, &sphereVbo, &sphereIbo, &orbInstanceBuffer);
    configure(torusMesh, &torusVbo, &torusIbo, &torusInstanceBuffer);

    SceneData sceneDataUBO;
    render::UniformBuffer<SceneData> ubo;
    ubo.bindTo(0);

    render::Framebuffer sceneFramebuffer;
    render::Texture2D sceneDepthTexture;
    render::Texture2D sceneOutputTexture;
    sceneOutputTexture.setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
    sceneOutputTexture.setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);
    sceneOutputTexture.setMinFilter(render::TextureMinFilter::LINEAR);
    sceneOutputTexture.setMagFilter(render::TextureMagFilter::LINEAR);
    auto status = sceneFramebuffer.setup({
        .width = 1280,
        .height = 720,
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
        .blurWidth = 1280,
        .blurHeight = 720,
        .outputWidth = 1920,
        .outputHeight = 1080,
    }};
    bloomEffect.setThreshold(1.0f);
    bloomEffect.setOffsetScale(1.2f);
    bloomEffect.setStrength(1.1f);

    bool applyBloom = true;
    bool splitView = true;
    bool animate = true;
    float threshold = bloomEffect.getThreshold();
    float strength = bloomEffect.getStrength();
    float offsetScale = bloomEffect.getOffsetScale();
    float emissiveGain = 8.0f;
    glm::ivec2 lastSize{1280, 720};

    util::TimeStep timeStep;
    util::SmoothFirstPersonCameraController camController;
    camController.moveSpeed = 12.0f;

    while (!render::window::shouldClose()) {
        core::input::update();
        render::ui::beginFrame();

        float runtime = static_cast<float>(render::window::getRuntime());
        if (timeStep.update(runtime)) {
            render::window::setTitle(
                std::string{"Bloom | FPS: "} +
                std::to_string(static_cast<int>(std::round(timeStep.fps())))
            );
        }

        if (isKeyDown(core::input::Key::ESCAPE)) {
            render::window::setShouldClose(true);
        }

        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10));
        ImGui::SetNextWindowSize(ImVec2(300, 250));
        ImGui::Begin("Bloom", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::TextWrapped("HDR neon orbs over a dark plaza. Bloom extracts bright pixels, blurs them, then composites.");
        ImGui::Separator();
        ImGui::Checkbox("Apply bloom", &applyBloom);
        ImGui::Checkbox("Split view (raw | bloom)", &splitView);
        ImGui::Checkbox("Animate", &animate);
        if (ImGui::SliderFloat("Threshold", &threshold, 0.2f, 4.0f)) {
            bloomEffect.setThreshold(threshold);
        }
        if (ImGui::SliderFloat("Strength", &strength, 0.0f, 3.0f)) {
            bloomEffect.setStrength(strength);
        }
        if (ImGui::SliderFloat("Blur width", &offsetScale, 0.2f, 4.0f)) {
            bloomEffect.setOffsetScale(offsetScale);
        }
        ImGui::SliderFloat("Emissive gain", &emissiveGain, 1.0f, 40.0f);
        ImGui::End();

        auto windowSize = render::window::getSize();
        if (windowSize != lastSize && windowSize.x > 0 && windowSize.y > 0) {
            sceneFramebuffer.resize(static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y));
            lastSize = windowSize;
        }

        float t = animate ? runtime : 0.0f;
        for (int i = 0; i < kOrbs; ++i) {
            float a = t * 0.4f + i * (2.0f * 3.14159265f / kOrbs);
            glm::vec3 pos{std::cos(a) * 7.0f, 1.2f + std::sin(t * 1.5f + i) * 0.6f, std::sin(a) * 7.0f};
            glm::vec3 rgb = util::HSVtoRGB(std::fmod(i / float(kOrbs) + t * 0.05f, 1.0f), 1.0f, 1.0f);
            orbInstances[i].color = glm::vec4{rgb * emissiveGain, 1.0f};
            orbInstances[i].model = trs(pos, glm::vec3{0.7f});
        }
        orbInstanceBuffer.setSubset(0, kOrbs, orbInstances.data());

        torusInstance.color = glm::vec4{glm::vec3{0.6f, 3.0f, 8.0f} * (emissiveGain / 12.0f), 1.0f};
        torusInstance.model = trs({0.0f, 2.0f, 0.0f}, glm::vec3{4.0f}, t * 0.35f, {0.2f, 1.0f, 0.1f});
        torusInstanceBuffer.setSubset(0, 1, &torusInstance);

        camController.update(camera, timeStep.dt());

        sceneDataUBO.viewPos = camera.getPosition();
        sceneDataUBO.view = camera.getViewMatrix();
        sceneDataUBO.projection = glm::perspective(
            glm::radians(camera.fov),
            (float)windowSize.x / (float)windowSize.y,
            camera.minDist, camera.maxDist
        );
        ubo.setData(sceneDataUBO);

        sceneFramebuffer.bind();
        meshShader.use();
        meshShader.setBool("applyTexture", false);
        boxMesh.render();
        orbMesh.render();
        torusMesh.render();

        if (applyBloom) {
            const auto& bloomTexture = bloomEffect.apply(sceneOutputTexture);
            render::Framebuffer::bindDefault(windowSize.x, windowSize.y, {
                .clearColor = true, .clearStencil = false, .clearDepth = false
            });
            if (splitView) {
                util::renderScreenQuadSplit(sceneOutputTexture, bloomTexture);
            } else {
                util::renderScreenQuadTexture(bloomTexture);
            }
        } else {
            render::Framebuffer::bindDefault(windowSize.x, windowSize.y, {
                .clearColor = true, .clearStencil = false, .clearDepth = false
            });
            util::renderScreenQuadTexture(sceneOutputTexture);
        }

        render::ui::endFrame();
        render::window::swapBuffers();
    }

    render::ui::terminate();
    return 0;
}
