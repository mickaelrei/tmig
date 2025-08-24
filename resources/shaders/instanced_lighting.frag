#version 440 core

out vec4 FragColor;

// Inputs from vertex shader
in vec3 fragNormal;
in vec3 fragPos;
in vec3 objectColor;

// Mesh data
uniform float specularStrength = 0.5f;
uniform int shininess = 32;

// Scene UBO
layout(std140, binding = 0) uniform Scene {
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

// Light Structs
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

// Lights UBO
layout(std140, binding = 1) uniform Lights {
    DirectionalLight directionalLights[64];
    PointLight pointLights[64];
    SpotLight spotLights[32];
    int numDirectionalLights;
    int numPointLights;
    int numSpotLights;
};

// Calculates color for a single directional light
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0f);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    
    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec * specularStrength;
    return (diffuse + specular) * light.intensity;
}

// Calculates color for a single point light
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0f);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec * specularStrength;
    return (diffuse + specular) * attenuation * light.intensity;
}

// Calculates color for a single spotlight
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    
    // Ensure fragment is inside the spotlight's cone
    if (theta <= light.outerCutOff) {
        return vec3(0.0f);
    }
     
    // Attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    
    // Soft edge
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

    vec3 diffuse = light.color * diff;
    vec3 specular = light.color * spec * specularStrength;
    return (diffuse + specular) * attenuation * intensity * light.intensity;
}

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 result = vec3(0.0f);

    // Ambient light
    vec3 ambient = vec3(0.1f) * objectColor;

    // Sum contributions from all lights
    for (int i = 0; i < numDirectionalLights; i++) {
        result += calculateDirectionalLight(directionalLights[i], norm, viewDir);
    }
    for (int i = 0; i < numPointLights; i++) {
        result += calculatePointLight(pointLights[i], norm, fragPos, viewDir);
    }
    for (int i = 0; i < numSpotLights; i++) {
        result += calculateSpotLight(spotLights[i], norm, fragPos, viewDir);
    }

    FragColor = vec4((ambient + result) * objectColor, 1.0);
}