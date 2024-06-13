#include "gl/shader.hpp"

// TODO: These should be just static const variables that get initialized after glfwInit,
//       because creating a new one for each object is not necessary

Shader newDefaultEntityShader();
Shader newSkyboxShader();