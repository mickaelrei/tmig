#include "gl/shader.hpp"

namespace tmig {

namespace utils {

// TODO: These should be just static const variables that get initialized after glfwInit,
//       because creating a new one for each object is not necessary

gl::Shader newDefaultEntityShader();
gl::Shader newSkyboxShader();

} // namespace utils

} /// namespace tmig