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
#include "tmig/render/ui.hpp"
#include "tmig/util/camera_controller.hpp"
#include "tmig/util/shapes.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/time_step.hpp"
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

struct QuadVert {
    glm::vec3 pos;
    glm::vec2 uv;
};

struct SceneData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 viewPos;
};

static glm::mat4 trs(const glm::vec3& pos, const glm::vec3& scale, float angle = 0.0f) {
    glm::mat4 m{1.0f};
    m = glm::translate(m, pos);
    if (angle != 0.0f) {
        m = glm::rotate(m, angle, glm::vec3{0.0f, 1.0f, 0.0f});
    }
    m = glm::scale(m, scale);
    return m;
}

int main() {
    srand(7);

    render::init();
    render::ui::init();
    render::window::setSize({1280, 720});
    render::setClearColor(glm::vec4{0.05f, 0.06f, 0.08f, 1.0f});

    render::Camera camera;
    camera.setPosition(glm::vec3{0.0f, 3.0f, 10.0f});

    render::ShaderProgram shader;
    if (!shader.compileFromFiles(
        util::getResourcePath("shaders/instanced.vert"),
        util::getResourcePath("shaders/instanced.frag")
    )) {
        std::cerr << "Failed loading instanced shader\n";
        return 1;
    }

    render::ShaderProgram postProcessingShader;
    if (!postProcessingShader.compileFromFiles(
        util::getResourcePath("engine/shaders/screen_quad.vert"),
        util::getResourcePath("shaders/post_processing.frag")
    )) {
        std::cerr << "Failed loading post_processing shader\n";
        return 1;
    }

    render::Texture2D texture;
    if (!texture.loadFromFile(util::getResourcePath("images/awesomeface.png"))) {
        std::cerr << "Failed to load texture\n";
        return 1;
    }
    texture.setWrapS(render::TextureWrapMode::MIRRORED_REPEAT);
    texture.setWrapT(render::TextureWrapMode::MIRRORED_REPEAT);
    texture.setMinFilter(render::TextureMinFilter::LINEAR_MIPMAP_LINEAR);
    texture.setMagFilter(render::TextureMagFilter::LINEAR);
    texture.generateMipmaps();

    std::vector<InstanceData> instances;

    // Floor
    instances.push_back({
        glm::vec4{0.55f, 0.55f, 0.6f, 1.0f},
        trs({0.0f, -1.5f, 0.0f}, {24.0f, 0.3f, 24.0f})
    });

    // Ring of textured cubes
    constexpr int kCubes = 8;
    for (int i = 0; i < kCubes; ++i) {
        float a = i * (2.0f * 3.14159265f / kCubes);
        instances.push_back({
            glm::vec4{1.0f},
            trs({std::cos(a) * 5.0f, 0.0f, std::sin(a) * 5.0f}, {1.4f, 1.4f, 1.4f}, a)
        });
    }

    // Center pedestal
    instances.push_back({
        glm::vec4{0.8f, 0.75f, 0.7f, 1.0f},
        trs({0.0f, -0.6f, 0.0f}, {2.0f, 1.2f, 2.0f})
    });

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
    boxInstanceBuffer.setData(instances);

    InstanceData torusInstance{
        glm::vec4{0.3f, 0.7f, 1.0f, 1.0f},
        trs({0.0f, 1.2f, 0.0f}, glm::vec3{1.8f})
    };
    render::DataBuffer<InstanceData> torusInstanceBuffer;
    torusInstanceBuffer.setData(&torusInstance, 1);

    render::InstancedMesh<Vertex, InstanceData> boxMesh;
    boxMesh.setAttributes({
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT2,
    }, {
        render::VertexAttributeType::FLOAT4,
        render::VertexAttributeType::MAT4x4,
    });
    boxMesh.setVertexBuffer(&boxVbo);
    boxMesh.setIndexBuffer(&boxIbo);
    boxMesh.setInstanceBuffer(&boxInstanceBuffer);

    render::InstancedMesh<Vertex, InstanceData> torusMesh;
    torusMesh.setAttributes({
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT2,
    }, {
        render::VertexAttributeType::FLOAT4,
        render::VertexAttributeType::MAT4x4,
    });
    torusMesh.setVertexBuffer(&torusVbo);
    torusMesh.setIndexBuffer(&torusIbo);
    torusMesh.setInstanceBuffer(&torusInstanceBuffer);

    std::vector<QuadVert> quadVertices;
    std::vector<uint32_t> quadIndices;
    util::generateScreenQuadMesh([&](auto v) {
        quadVertices.push_back(QuadVert{v.position, v.uv});
    }, quadIndices);

    render::DataBuffer<QuadVert> screenQuadVbo;
    render::DataBuffer<uint32_t> screenQuadIbo;
    screenQuadVbo.setData(quadVertices);
    screenQuadIbo.setData(quadIndices);

    render::Mesh<QuadVert> screenQuadMesh;
    screenQuadMesh.setAttributes({
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT2,
    });
    screenQuadMesh.setVertexBuffer(&screenQuadVbo);
    screenQuadMesh.setIndexBuffer(&screenQuadIbo);

    SceneData sceneDataUBO;
    render::UniformBuffer<SceneData> ubo;
    ubo.bindTo(0);

    render::Texture2D sceneOutputTexture;
    render::Texture2D sceneDepthTexture;
    sceneOutputTexture.setWrapS(render::TextureWrapMode::CLAMP_TO_EDGE);
    sceneOutputTexture.setWrapT(render::TextureWrapMode::CLAMP_TO_EDGE);

    render::Framebuffer fb;
    auto status = fb.setup({
        .width = 1920,
        .height = 1080,
        .colorAttachments = {
            {0, render::FramebufferAttachment{
                .texture = &sceneOutputTexture,
                .format = render::TextureFormat::RGBA8,
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

    const char* effects[] = {
        "None", "Sharpen", "Outline", "Emboss", "Blur",
        "Invert", "Grayscale", "Chromatic aberration", "Vignette"
    };
    int effect = 2;
    float intensity = 1.0f;
    float offset = 1.0f / 600.0f;
    bool splitView = true;
    bool animate = true;
    glm::ivec2 lastSize{1280, 720};

    util::TimeStep timeStep;
    util::SmoothFirstPersonCameraController camController;
    camController.moveSpeed = 8.0f;

    while (!render::window::shouldClose()) {
        core::input::update();
        render::ui::beginFrame();

        float runtime = static_cast<float>(render::window::getRuntime());
        if (timeStep.update(runtime)) {
            render::window::setTitle(
                std::string{"Framebuffer | "} + effects[effect] + " | FPS: " +
                std::to_string(static_cast<int>(std::round(timeStep.fps())))
            );
        }

        if (isKeyDown(core::input::Key::ESCAPE)) {
            render::window::setShouldClose(true);
        }

        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10));
        ImGui::SetNextWindowSize(ImVec2(340, 250));
        ImGui::Begin("Framebuffer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::TextWrapped(
            "Scene is rendered off-screen into a Framebuffer texture, then a screen-space "
            "pass samples that texture to apply a kernel / color effect."
        );
        ImGui::Separator();
        ImGui::Combo("Effect", &effect, effects, IM_ARRAYSIZE(effects));
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 1.5f);
        ImGui::SliderFloat("Kernel offset", &offset, 0.0002f, 0.01f, "%.4f");
        ImGui::Checkbox("Split view (raw | processed)", &splitView);
        ImGui::Checkbox("Animate", &animate);
        ImGui::Text("FBO %ux%u  |  window %dx%d", fb.width(), fb.height(), lastSize.x, lastSize.y);
        ImGui::End();

        auto windowSize = render::window::getSize();
        if (windowSize != lastSize && windowSize.x > 0 && windowSize.y > 0) {
            fb.resize(static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y));
            lastSize = windowSize;
        }

        float t = animate ? runtime : 0.0f;
        for (int i = 0; i < kCubes; ++i) {
            float a = i * (2.0f * 3.14159265f / kCubes) + t * 0.25f;
            float y = 0.2f + std::sin(t * 1.4f + i) * 0.35f;
            instances[1 + i].model = trs(
                {std::cos(a) * 5.0f, y, std::sin(a) * 5.0f},
                {1.4f, 1.4f, 1.4f},
                a + t
            );
        }
        boxInstanceBuffer.setSubset(1, kCubes, instances.data() + 1);

        torusInstance.model = trs({0.0f, 1.2f + std::sin(t) * 0.15f, 0.0f}, glm::vec3{1.8f}, t * 0.7f);
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

        fb.bind();
        shader.use();
        shader.setBool("applyTexture", true);
        shader.setTexture("tex", texture, 0);
        boxMesh.render();
        torusMesh.render();

        render::Framebuffer::bindDefault(windowSize.x, windowSize.y, {
            .clearColor = true, .clearStencil = false, .clearDepth = false
        });

        postProcessingShader.use();
        postProcessingShader.setInt("effect", effect);
        postProcessingShader.setFloat("intensity", intensity);
        postProcessingShader.setFloat("offset", offset);
        postProcessingShader.setBool("splitView", splitView);
        postProcessingShader.setTexture("scene", sceneOutputTexture, 0);

        glDisable(GL_DEPTH_TEST);
        screenQuadMesh.render();
        glEnable(GL_DEPTH_TEST);

        render::ui::endFrame();
        render::window::swapBuffers();
    }

    render::ui::terminate();
    return 0;
}
