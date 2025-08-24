#version 440 core

// Input attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// General info UBO
layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

// Model-specific uniform
uniform mat4 model;

// Outputs to fragment shader
out vec3 fragNormal;
out vec3 fragPos;

void main() {
    // Transform normal to world space
    fragNormal = transpose(inverse(mat3(model))) * aNormal;

    // Transform position to world space
    fragPos = vec3(model * vec4(aPos, 1.0f));

    // Final vertex position
    gl_Position = projection * view * vec4(fragPos, 1.0f);
}