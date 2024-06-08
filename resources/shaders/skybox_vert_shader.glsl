#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoord = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w - 1e-4f, pos.w);
}