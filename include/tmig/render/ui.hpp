#pragma once

namespace tmig::render::ui {

/// @brief Initializes the Dear ImGui context and backends
/// @note Must be called after `tmig::render::init()`.
void init();

/// @brief Terminates the Dear ImGui context
void terminate();

/// @brief Starts a new ImGui frame. Call this once at the beginning of your main loop
void beginFrame();

/// @brief Renders the ImGui draw data. Call this once at the end of your main loop after rendering your 3D scene
void endFrame();

}