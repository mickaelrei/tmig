#version 440 core
out vec4 FragColor;

in vec2 uv;

// Original scene
uniform sampler2D scene;

// Blurred excess (bloom)
uniform sampler2D bloomBlur;

// Bloom strength
uniform float strength = 1.0f;

void main() {
    vec4 sceneColor = texture(scene, uv);
    vec4 bloomColor = texture(bloomBlur, uv);
    FragColor = sceneColor + bloomColor * strength;
}