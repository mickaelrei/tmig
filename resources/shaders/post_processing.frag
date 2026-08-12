#version 440
out vec4 FragColor;

in vec2 uv;

uniform sampler2D scene;
uniform int effect;
uniform float offset = 0.0017;
uniform float intensity = 1.0;
uniform bool splitView = false;

const float defaultKernel[9] = float[](
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
);

const float sharpenKernel[9] = float[](
    0, -1, 0,
    -1,  5, -1,
    0, -1, 0
);

const float outlineKernel[9] = float[](
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

const float embossKernel[9] = float[](
    -2, -1, 0,
    -1,  1, 1,
     0,  1, 2
);

const float blurKernel[9] = float[](
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
    2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
);

vec3 applyKernel(float kernel[9]) {
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0,     offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0),
        vec2( 0.0,     0.0),
        vec2( offset,  0.0),
        vec2(-offset, -offset),
        vec2( 0.0,    -offset),
        vec2( offset, -offset)
    );

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        col += texture(scene, uv + offsets[i]).rgb * kernel[i];
    }
    return col;
}

vec3 applyEffect(vec3 original) {
    if (effect == 0) {
        return original;
    } else if (effect == 1) {
        return mix(original, applyKernel(sharpenKernel), intensity);
    } else if (effect == 2) {
        return mix(original, applyKernel(outlineKernel), intensity);
    } else if (effect == 3) {
        return mix(original, applyKernel(embossKernel), intensity);
    } else if (effect == 4) {
        return mix(original, applyKernel(blurKernel), intensity);
    } else if (effect == 5) {
        return mix(original, 1.0 - original, intensity);
    } else if (effect == 6) {
        float gray = dot(original, vec3(0.2126, 0.7152, 0.0722));
        return mix(original, vec3(gray), intensity);
    } else if (effect == 7) {
        float aberr = offset * 8.0 * intensity;
        return vec3(
            texture(scene, uv + vec2(aberr, 0.0)).r,
            original.g,
            texture(scene, uv - vec2(aberr, 0.0)).b
        );
    } else if (effect == 8) {
        float dist = distance(uv, vec2(0.5));
        float vig = smoothstep(0.85, 0.25, dist);
        return original * mix(1.0, vig, intensity);
    }

    return applyKernel(defaultKernel);
}

void main() {
    vec3 original = texture(scene, uv).rgb;

    if (splitView && uv.x < 0.5) {
        FragColor = vec4(original, 1.0);
        // Separator line
        if (uv.x > 0.497) {
            FragColor = vec4(1.0);
        }
        return;
    }

    FragColor = vec4(applyEffect(original), 1.0);
}
