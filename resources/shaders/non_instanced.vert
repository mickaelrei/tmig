#version 440 core

// Input attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// General info
layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

// Outputs
out vec3 fragNormal;
out vec3 fragPos;
out vec4 meshColor;

// Renderable inputs
uniform vec4 color;
uniform mat4 model;

void main() {
    fragNormal = transpose(inverse(mat3(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0f));
    meshColor = color;

    vec4 viewModel = view * model * vec4(aPos, 1.0f);
    gl_Position = projection * viewModel;
}