#include "tmig/gl/gl.hpp"

namespace tmig {

namespace gl {

Shader skyboxShader() {
    static Shader shader{
        "resources/shaders/skybox_vert_shader.glsl",
        "resources/shaders/skybox_frag_shader.glsl"
    };
    return shader;
}

Shader entityShader() {
    static Shader shader{
        "resources/shaders/entity_vert_shader.glsl",
        "resources/shaders/entity_frag_shader.glsl"
    };
    return shader;
}

} // namespace gl

} // namespace tmig
