#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 color;
layout (location = 4) in mat4 model;

layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

out vec3 fragNormal;
out vec3 fragPos;
out vec4 meshColor;
out vec2 texUV;

void main() {
    fragNormal = transpose(inverse(mat3(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));
    texUV = aTex;
    meshColor = color;

    gl_Position = projection * view * vec4(fragPos, 1.0);
}
