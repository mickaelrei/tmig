#version 440 core
out vec4 FragColor;

in vec2 uv;

uniform int applyBloom;
uniform sampler2D scene;
uniform sampler2D bloomBlur;

void main() {
    vec4 sceneColor = texture(scene, uv);
    vec4 bloomColor = texture(bloomBlur, uv);
    FragColor = sceneColor + bloomColor * applyBloom;
}