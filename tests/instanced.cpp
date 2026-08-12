#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <chrono>

#include "tmig/render/instanced_mesh.hpp"
#include "tmig/render/mesh.hpp"
#include "tmig/render/uniform_buffer.hpp"
#include "tmig/render/render.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/window.hpp"
#include "tmig/render/texture2D.hpp"
#include "tmig/render/ui.hpp"
#include "tmig/util/camera_controller.hpp"
#include "tmig/util/resources.hpp"
#include "tmig/util/shapes.hpp"
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
    glm::vec4 posSeed;
    glm::vec4 scale;
};

struct SceneData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 viewPos;
};

int main() {
    srand(3);

    render::init();
    render::ui::init();
    render::setVSync(false);
    render::window::setSize({1280, 720});
    render::setClearColor(glm::vec4{0.02f, 0.02f, 0.03f, 1.0f});

    render::Camera camera;
    camera.maxDist = 10000.0f;
    camera.setPosition(glm::vec3{0.0f, 8.0f, 20.0f});

    render::ShaderProgram instancedShader;
    if (!instancedShader.compileFromFiles(
        util::getResourcePath("shaders/instanced_perf.vert"),
        util::getResourcePath("shaders/instanced.frag")
    )) {
        std::cerr << "Failed loading instanced shader\n";
        return 1;
    }

    render::ShaderProgram nonInstancedShader;
    if (!nonInstancedShader.compileFromFiles(
        util::getResourcePath("shaders/non_instanced_perf.vert"),
        util::getResourcePath("shaders/non_instanced_perf.frag")
    )) {
        std::cerr << "Failed loading non-instanced shader\n";
        return 1;
    }

    render::Texture2D texture;
    if (!texture.loadFromFile(util::getResourcePath("images/container.jpg"))) {
        std::cerr << "Failed to load texture\n";
        return 1;
    }
    texture.setWrapS(render::TextureWrapMode::MIRRORED_REPEAT);
    texture.setWrapT(render::TextureWrapMode::MIRRORED_REPEAT);
    texture.setMinFilter(render::TextureMinFilter::LINEAR_MIPMAP_LINEAR);
    texture.setMagFilter(render::TextureMagFilter::LINEAR);
    texture.generateMipmaps();

    constexpr int kMaxInstances = 200000;
    std::vector<InstanceData> instances(kMaxInstances);
    for (int i = 0; i < kMaxInstances; ++i) {
        instances[i].color = {
            (rand() % 1000) / 1000.0f,
            (rand() % 1000) / 1000.0f,
            (rand() % 1000) / 1000.0f,
            1.0f
        };
        instances[i].posSeed = {
            static_cast<float>(rand() % 1000 - 500) * 2.0f,
            static_cast<float>(rand() % 1000 - 500) * 2.0f,
            static_cast<float>(rand() % 1000 - 500) * 2.0f,
            static_cast<float>(i) * 0.01f
        };
        instances[i].scale = {
            (rand() % 900 + 100) / 100.0f,
            (rand() % 900 + 100) / 100.0f,
            (rand() % 900 + 100) / 100.0f,
            0.0f
        };
    }

    render::DataBuffer<Vertex> boxVbo, sphereLowVbo, sphereHighVbo;
    render::DataBuffer<uint32_t> boxIbo, sphereLowIbo, sphereHighIbo;

    std::vector<Vertex> boxVerts;
    std::vector<uint32_t> boxIdx;
    util::generateBoxMesh([&](auto v) {
        boxVerts.push_back(Vertex{v.position, v.normal, v.uv});
    }, boxIdx);
    boxVbo.setData(boxVerts);
    boxIbo.setData(boxIdx);

    auto [lowVerts, lowIdx] = [&] {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        util::generateSphereMesh([&](auto v) {
            vertices.push_back(Vertex{v.position, v.normal, v.uv});
        }, indices, 3);
        sphereLowVbo.setData(vertices);
        sphereLowIbo.setData(indices);
        return std::pair<size_t, size_t>{vertices.size(), indices.size()};
    }();

    auto [highVerts, highIdx] = [&] {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        util::generateSphereMesh([&](auto v) {
            vertices.push_back(Vertex{v.position, v.normal, v.uv});
        }, indices, 24);
        sphereHighVbo.setData(vertices);
        sphereHighIbo.setData(indices);
        return std::pair<size_t, size_t>{vertices.size(), indices.size()};
    }();

    int instanceCount = 50000;
    render::DataBuffer<InstanceData> instanceBuffer;
    instanceBuffer.setData(instances.data(), static_cast<size_t>(instanceCount));

    const std::vector<render::VertexAttributeType> vertexLayout{
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT3,
        render::VertexAttributeType::FLOAT2,
    };
    const std::vector<render::VertexAttributeType> instanceLayout{
        render::VertexAttributeType::FLOAT4,
        render::VertexAttributeType::FLOAT4,
        render::VertexAttributeType::FLOAT4,
    };

    render::InstancedMesh<Vertex, InstanceData> instancedMesh;
    instancedMesh.setAttributes(vertexLayout, instanceLayout);
    instancedMesh.setVertexBuffer(&boxVbo);
    instancedMesh.setIndexBuffer(&boxIbo);
    instancedMesh.setInstanceBuffer(&instanceBuffer);

    render::Mesh<Vertex> mesh;
    mesh.setAttributes(vertexLayout);
    mesh.setVertexBuffer(&boxVbo);
    mesh.setIndexBuffer(&boxIbo);

    SceneData sceneDataUBO;
    render::UniformBuffer<SceneData> ubo;
    ubo.bindTo(0);

    bool useInstancing = true;
    bool animate = true;
    bool applyTexture = true;
    bool wireframe = false;
    bool vsync = false;
    int meshKind = 0;
    int lastCount = instanceCount;
    int lastMeshKind = meshKind;
    float submitMs = 0.0f;

    util::TimeStep timeStep;
    util::SmoothFirstPersonCameraController camController;
    camController.moveSpeed = 80.0f;

    auto bindMeshKind = [&](int kind) {
        render::DataBuffer<Vertex>* vbo = &boxVbo;
        render::DataBuffer<uint32_t>* ibo = &boxIbo;
        if (kind == 1) {
            vbo = &sphereLowVbo;
            ibo = &sphereLowIbo;
        } else if (kind == 2) {
            vbo = &sphereHighVbo;
            ibo = &sphereHighIbo;
        }
        instancedMesh.setVertexBuffer(vbo);
        instancedMesh.setIndexBuffer(ibo);
        mesh.setVertexBuffer(vbo);
        mesh.setIndexBuffer(ibo);
    };

    while (!render::window::shouldClose()) {
        core::input::update();
        render::ui::beginFrame();

        float runtime = static_cast<float>(render::window::getRuntime());
        if (timeStep.update(runtime)) {
            const char* mode = useInstancing ? "Instanced" : "Non-instanced";
            render::window::setTitle(
                std::string{"Instancing | "} + mode + " | " +
                std::to_string(instanceCount) + " | FPS: " +
                std::to_string(static_cast<int>(std::round(timeStep.fps())))
            );
        }

        if (isKeyDown(core::input::Key::ESCAPE)) {
            render::window::setShouldClose(true);
        }

        const char* meshNames[] = { "Box (12 tris)", "Sphere low", "Sphere high" };
        size_t meshIdxCount = boxIdx.size();
        size_t meshVertCount = boxVerts.size();
        if (meshKind == 1) {
            meshIdxCount = lowIdx;
            meshVertCount = lowVerts;
        } else if (meshKind == 2) {
            meshIdxCount = highIdx;
            meshVertCount = highVerts;
        }
        const uint64_t triangles = static_cast<uint64_t>(instanceCount) * (meshIdxCount / 3);

        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10));
        ImGui::SetNextWindowSize(ImVec2(360, 320));
        ImGui::Begin("Instancing", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::TextWrapped(
            "Instancing is one draw call. Non-instanced is one call per mesh. "
            "Animation runs on the GPU so CPU matrix updates don't hide the difference. "
            "VSync off so FPS can exceed the monitor refresh."
        );
        ImGui::Separator();
        ImGui::Checkbox("Instanced rendering", &useInstancing);
        if (ImGui::Checkbox("VSync", &vsync)) {
            render::setVSync(vsync);
        }
        ImGui::Checkbox("Animate (GPU)", &animate);
        ImGui::Checkbox("Texture", &applyTexture);
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::Combo("Mesh", &meshKind, meshNames, IM_ARRAYSIZE(meshNames));
        ImGui::SliderInt("Count", &instanceCount, 1, kMaxInstances, "%d", ImGuiSliderFlags_Logarithmic);
        ImGui::Separator();
        ImGui::Text("Draw calls: %d", useInstancing ? 1 : instanceCount);
        ImGui::Text("Mesh: %zu verts, %zu idx", meshVertCount, meshIdxCount);
        ImGui::Text("Triangles: %llu", static_cast<unsigned long long>(triangles));
        ImGui::Text("CPU submit: %.2f ms", submitMs);
        ImGui::Text("FPS: %.0f", timeStep.fps());
        if (!useInstancing && instanceCount > 20000) {
            ImGui::TextWrapped("Non-instanced with this count is CPU-bound on draw calls. That's the point.");
        }
        if (meshKind == 2 && instanceCount > 8000) {
            ImGui::TextWrapped("High-poly spheres are GPU vertex-bound; both paths will look similar.");
        }
        ImGui::End();

        if (instanceCount != lastCount) {
            instanceBuffer.setData(instances.data(), static_cast<size_t>(instanceCount));
            lastCount = instanceCount;
        }
        if (meshKind != lastMeshKind) {
            bindMeshKind(meshKind);
            lastMeshKind = meshKind;
        }

        camController.update(camera, timeStep.dt());

        auto windowSize = render::window::getSize();
        sceneDataUBO.viewPos = camera.getPosition();
        sceneDataUBO.view = camera.getViewMatrix();
        sceneDataUBO.projection = glm::perspective(
            glm::radians(camera.fov),
            windowSize.y > 0 ? (float)windowSize.x / (float)windowSize.y : 1.0f,
            camera.minDist, camera.maxDist
        );
        ubo.setData(sceneDataUBO);

        render::clearBuffers();
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        const auto submitStart = std::chrono::steady_clock::now();
        if (useInstancing) {
            instancedShader.use();
            instancedShader.setBool("applyTexture", applyTexture);
            instancedShader.setBool("uAnimate", animate);
            instancedShader.setFloat("uTime", runtime);
            instancedShader.setTexture("tex", texture, 0);
            instancedMesh.render();
        } else {
            nonInstancedShader.use();
            nonInstancedShader.setBool("uAnimate", animate);
            nonInstancedShader.setFloat("uTime", runtime);
            for (int i = 0; i < instanceCount; ++i) {
                nonInstancedShader.setVec4("color", instances[i].color);
                nonInstancedShader.setVec4("posSeed", instances[i].posSeed);
                nonInstancedShader.setVec4("scalePad", instances[i].scale);
                mesh.render();
            }
        }
        submitMs = std::chrono::duration<float, std::milli>(
            std::chrono::steady_clock::now() - submitStart
        ).count();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        render::ui::endFrame();
        render::window::swapBuffers();
    }

    render::ui::terminate();
    return 0;
}
