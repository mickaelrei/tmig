#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main()
{
    texCoord = aTexCoord;
    normal = transpose(inverse(mat3(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0f));

    vec4 viewPos = view * model * vec4(aPos, 1.0f);

    // Simulate world curvature
    // float dist = length(viewPos.xyz);
    // float curved = viewPos.y - 0.025f * dist * dist;
    // viewPos.y = curved;

    gl_Position = projection * viewPos;
}