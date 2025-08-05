#version 440
out vec4 FragColor;

in vec3 pos;
in vec2 uv;

// Scene texture
uniform sampler2D scene;
uniform int effect;

// Whats the offset for neighboring texels
const float offset = 1.0 / 600.0;

// Kernel for default image
const float defaultKernel[9] = float[](
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
);

// Kernel for sharpen effect
const float sharpenKernel[9] = float[](
    0, -1, 0,
    -1,  5, -1,
    0, -1, 0
);

// Kernel for outline effect
const float outlineKernel[9] = float[](
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

// Kernel for emboss effect
const float embossKernel[9] = float[](
    -2, -1, 0,
    -1,  1, 1,
     0,  1, 2
);

// Kernel for blur effect
float blurKernel[9] = float[](
    1.0 / 16.0f, 2.0 / 16.0f, 1.0 / 16.0f,
    2.0 / 16.0f, 4.0 / 16.0f, 2.0 / 16.0f,
    1.0 / 16.0f, 2.0 / 16.0f, 1.0 / 16.0f
);

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right
    );

    float kernel[9];
    if      (effect == 0) kernel = defaultKernel;
    else if (effect == 1) kernel = sharpenKernel;
    else if (effect == 2) kernel = outlineKernel;
    else if (effect == 3) kernel = embossKernel;
    else                  kernel = blurKernel;

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        col += vec3(texture(scene, uv + offsets[i])) * kernel[i];
    }

    FragColor = vec4(col, 1.0);
}  