#include "utils/shaders.hpp"

namespace tmig {

namespace utils {

gl::Shader newDefaultEntityShader()
{
    return gl::Shader{
        "resources/shaders/vertex_shader.glsl",
        "resources/shaders/fragment_shader.glsl"
    };
}

gl::Shader newSkyboxShader()
{
    return gl::Shader{
        "resources/shaders/skybox_vert_shader.glsl",
        "resources/shaders/skybox_frag_shader.glsl"
    };
}

} // namespace gl

} // namespace tmig