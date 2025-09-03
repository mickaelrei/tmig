#ifdef DEBUG
#include <stdexcept>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "tmig/render/internal.hpp"
#include "tmig/util/log.hpp"

namespace tmig::render::ui {

static bool initialized = false;

void init() {
    if (initialized) return;
    initialized = true;

    const auto window = tmig::render::window::getGlfwWindow();
#ifdef DEBUG
    if (window == nullptr) {
        throw std::runtime_error{"[render::ui::init] Render module is not initialized"};
    }
#endif

    // Create ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    // When viewports are enabled, we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // 3. Setup Platform/Renderer backends using our internal getter
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440");
}

void terminate() {
    if (!initialized) return;
    initialized = false;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void beginFrame() {
    if (!initialized) {
        util::logMessage(
            util::LogCategory::ENGINE, util::LogSeverity::WARNING,
            "render::ui::beginFrame called when render::ui is not initialized. Call render::ui::init() first"
        );
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void endFrame() {
    if (!initialized) {
        util::logMessage(
            util::LogCategory::ENGINE, util::LogSeverity::WARNING,
            "render::ui::endFrame called when render::ui is not initialized. Call render::ui::init() first"
        );
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

} // namespace tmig::render::ui
