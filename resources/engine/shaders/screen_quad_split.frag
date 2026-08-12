#version 440 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;
uniform sampler2D processed;

void main() {
    if (uv.x < 0.5) {
        FragColor = texture(scene, uv);
        if (uv.x > 0.498) {
            FragColor = vec4(1.0);
        }
    } else {
        FragColor = texture(processed, uv);
    }
}
