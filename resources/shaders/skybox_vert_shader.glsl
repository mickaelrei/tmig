#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoord = aPos;
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}