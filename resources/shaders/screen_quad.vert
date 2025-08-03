#version 440
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 pos;
out vec2 uv;

void main() {
    pos = aPos;
    uv = aTex;
    gl_Position = vec4(aPos, 1.0f);
}
