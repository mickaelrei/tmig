#include "utils/shaders.hpp"

Shader newDefaultEntityShader()
{
    return Shader{"resources/shaders/vertex_shader.glsl", "resources/shaders/fragment_shader.glsl"};
}

Shader newSkyboxShader()
{
    return Shader{"resources/shaders/skybox_vert_shader.glsl", "resources/shaders/skybox_frag_shader.glsl"};
}
