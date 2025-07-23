#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 color;
layout (location = 4) in mat4 model;

out vec3 fragNormal;
out vec3 fragPos;
out vec4 meshColor;
out vec2 texUV;

uniform mat4 view;
uniform mat4 projection;

void main() {
    fragNormal = transpose(inverse(mat3(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0f));
    texUV = aTex;
    meshColor = color;

    vec4 viewModel = view * model * vec4(aPos, 1.0f);
    gl_Position = projection * viewModel;
}