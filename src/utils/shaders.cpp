#include "tmig/utils/shaders.hpp"

namespace tmig {

namespace utils {

std::shared_ptr<gl::Shader> skyboxShader() {
    static auto shader = gl::Shader::create(
        "resources/shaders/skybox_vert_shader.glsl",
        "resources/shaders/skybox_frag_shader.glsl"
    );
    return shader;
}

std::shared_ptr<gl::Shader> entityShader() {
    static auto shader = gl::Shader::create(
        "resources/shaders/entity_vert_shader.glsl",
        "resources/shaders/entity_frag_shader.glsl"
    );
    return shader;
}

} // namespace utils

} // namespace tmig
