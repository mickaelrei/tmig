#version 440 core
out vec4 FragColor;

in vec2 uv;

// Whether to apply blur horizontally or vertically
uniform bool horizontal;

// Texture sample offset scale
uniform float offsetScale = 1.0f;

// Incoming image
uniform sampler2D image;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {             
    vec2 texOffset = offsetScale / textureSize(image, 0);
    vec3 result = texture(image, uv).rgb * weight[0];
    if (horizontal) {
        for (int i = 1; i < 5; ++i) {
            result += texture(image, uv + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, uv - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else {
        for (int i = 1; i < 5; ++i) {
            result += texture(image, uv + vec2(0.0, texOffset.y * i)).rgb * weight[i];
            result += texture(image, uv - vec2(0.0, texOffset.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0);
}