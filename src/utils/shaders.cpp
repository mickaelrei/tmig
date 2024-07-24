#include "tmig/utils/shaders.hpp"

namespace tmig {

namespace utils {

gl::Shader skyboxShader() {
    static gl::Shader shader{
        "resources/shaders/skybox_vert_shader.glsl",
        "resources/shaders/skybox_frag_shader.glsl"
    };
    return shader;
}

gl::Shader entityShader() {
    static gl::Shader shader{
        "resources/shaders/entity_vert_shader.glsl",
        "resources/shaders/entity_frag_shader.glsl"
    };
    return shader;
}

} // namespace utils

} // namespace tmig
