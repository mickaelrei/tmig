#pragma once

namespace tmig {

/// @brief Initializes everything needed by tmig engine
void init();

/// @brief Terminates everything which was initialized by tmig engine
void terminate();

/// @brief Initializes GLFW
void initGLFW();

/// @brief Initializes GLFW
void initGLAD();

/// @brief Initializes default objects such as shaders, uniform buffers, etc
void initDefaults();

/// @brief Terminates GLFW
void terminateGLFW();

}