#version 440 core
out vec4 FragColor;

// Vertex info
in vec3 fragNormal;
in vec3 fragPos;
in vec2 texUV;
in vec4 meshColor;

// General info
layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

// Renderable info
uniform bool applyTexture;
uniform sampler2D tex;
uniform float threshold = 1.0f;

// Directional light info
const float lightIntensity = 0.1f;
const float specularStrength = 0.15f;
const float ambientLightStrength = 0.15f;
const vec3 lightDirection = vec3(1.0f, -0.5f, -0.25f);
const vec3 lightColor = vec3(1.0f);

vec3 calculateLighting() {
    // Light diffuse
    vec3 normal = normalize(fragNormal);
    vec3 normalizedLightDirection = normalize(-lightDirection);
    float diffuse = max(dot(normal, normalizedLightDirection), 0.0f);

    // Light specular
    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 halfDirection = normalize(normalizedLightDirection + viewDirection);
    float specularAmount = pow(max(dot(viewDirection, halfDirection), 0.0f), 128);
    float specular = specularAmount * specularStrength;

    // Final light color
    return lightColor * ((diffuse + specular) * lightIntensity + ambientLightStrength);
}

void main() {
    vec4 color = meshColor;
    if (applyTexture) {
        color = texture(tex, texUV);
        // If texture alpha is not 100%, starting mixing with mesh color
        if (color.a < 1.0f) {
            color = vec4(mix(color.rgb, meshColor.rgb, 1.0f - color.a), meshColor.a);
        }
    }

    FragColor = color * vec4(calculateLighting(), 1.0f);
    // FragColor = vec4(fragNormal * 0.5f + 0.5f, 1.0f);
}
