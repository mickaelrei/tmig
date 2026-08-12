#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform vec4 color;
uniform mat4 model;

out vec3 fragNormal;
out vec3 fragPos;
out vec4 meshColor;

void main() {
    fragNormal = transpose(inverse(mat3(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));
    meshColor = color;

    gl_Position = projection * view * vec4(fragPos, 1.0);
}
