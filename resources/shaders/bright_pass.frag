#version 440 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;
uniform float threshold = 1.0f;

void main() {
    vec4 color = texture(scene, uv);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > threshold) {
        FragColor = color;
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}