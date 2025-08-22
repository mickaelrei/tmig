#version 440 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;

void main() {
    FragColor = texture(scene, uv);
}