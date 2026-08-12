#version 440 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec4 color;
layout (location = 4) in vec4 posSeed;
layout (location = 5) in vec4 scalePad;

layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform float uTime;
uniform bool uAnimate;

out vec3 fragNormal;
out vec3 fragPos;
out vec4 meshColor;
out vec2 texUV;

mat4 makeModel(vec3 t, vec3 s, float angle) {
    vec3 a = normalize(vec3(0.3, -0.7, 0.4));
    float c = cos(angle);
    float si = sin(angle);
    float ic = 1.0 - c;
    mat3 R = mat3(
        c + a.x * a.x * ic,       a.x * a.y * ic - a.z * si, a.x * a.z * ic + a.y * si,
        a.y * a.x * ic + a.z * si, c + a.y * a.y * ic,       a.y * a.z * ic - a.x * si,
        a.z * a.x * ic - a.y * si, a.z * a.y * ic + a.x * si, c + a.z * a.z * ic
    );
    return mat4(
        vec4(R[0] * s.x, 0.0),
        vec4(R[1] * s.y, 0.0),
        vec4(R[2] * s.z, 0.0),
        vec4(t, 1.0)
    );
}

void main() {
    vec3 pos = posSeed.xyz;
    vec3 scale = scalePad.xyz;
    float seed = posSeed.w;
    float angle = 0.0;

    if (uAnimate) {
        pos += vec3(sin(uTime + seed), cos(uTime - seed * 10.0), 0.0) * 8.0;
        angle = uTime + seed;
        float t = uTime * 2.0 + seed;
        scale *= vec3(sin(t) * 0.45 + 1.0, cos(t) * 0.45 + 1.0, sin(t + 1.0) * 0.45 + 1.0);
    }

    mat4 model = makeModel(pos, scale, angle);
    mat3 normalMat = mat3(model);

    fragNormal = normalMat * aNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));
    texUV = aTex;
    meshColor = color;

    gl_Position = projection * view * vec4(fragPos, 1.0);
}
