#include <stdexcept>
#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "tmig/init.hpp"

// Whether GLFW is initialized
static bool _initializedGLFW = false;

// Whether GLAD is initialized
static bool _initializedGLAD = false;

// Whether tmig defaults are initialized
static bool _initializedDefaults = false;

namespace tmig {

void init()
{
    if (!_initializedGLFW)
    {
        initGLFW();
    }

    if (!_initializedDefaults)
    {
        initDefaults();
    }
}

void terminate()
{
    if (_initializedGLFW)
    {
        terminateGLFW();
    }

    _initializedGLFW = false;
}

void initGLFW()
{
    // Init should be called only once
    if (_initializedGLFW) return;

    // Init GLFW
    if (glfwInit() == GLFW_FALSE)
    {
        throw std::runtime_error{"Failed to initialize GLFW\n"};
        return;
    }

    // Set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // If nothing fails, GLFW is initialized
    _initializedGLFW = true;
}

void initGLAD()
{
    // GLFW should be already initialized
    if (!_initializedGLFW) {
        std::cout << "WARNING [tmig::initGLAD()]: GLFW is not initialized. Make sure to call tmig::init() first\n";
    }

    // Init should be called only once
    if (_initializedGLAD) return;

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::runtime_error{"Failed to initialize GLAD\n"};
        return;
    }

    // If nothing fails, GLAD is initialized
    _initializedGLAD = true;
}

void initDefaults()
{
    // GLFW must be initialized before initializing tmig defaults
    if (!_initializedGLFW)
    {
        std::cout << "WARNING [tmig::initDefaults()]: GLFW is not initialized. Make sure to call tmig::init() first\n";
        return;
    }

    // Defaults should be initialized only once
    if (_initializedDefaults) return;

    // Initialize defaults

    _initializedDefaults = true;
}

void terminateGLFW()
{
    if (!_initializedGLFW)
    {
        std::cout << "WARNING [tmig::terminateGLFW()]: GLFW is not initialized, so it can't be terminated\n";
        return;
    }

    // Terminate
    glfwTerminate();
    _initializedGLFW = false;
}

}