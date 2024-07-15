#version 330 core
out vec4 FragColor;

// Light caster types
struct PointLight {
    int enabled;
    vec3 color;

    vec3 pos;
};

struct DirectionalLight {
    int enabled;
    vec3 color;

    vec3 dir;
};

struct SpotLight {
    int enabled;
    vec3 color;

    vec3 pos;
    vec3 dir;
    float cutoffAngle;
    float outerCutoffAngle;
};

// Vertex info
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

// Entity info
uniform vec4 meshColor;

// Entity textures
#define MAX_TEXTURES 4
uniform int numTextures;
uniform sampler2D textures[MAX_TEXTURES];

// General info
uniform vec3 viewPos;

// Lighting info
uniform float ambientStrength;
uniform float specularStrength;

// Scene lights
#define MAX_LIGHTS 50
uniform int numPointLights;
uniform int numDirectionalLights;
uniform int numSpotLights;
uniform PointLight pointLights[MAX_LIGHTS];
uniform DirectionalLight directionalLights[MAX_LIGHTS];
uniform SpotLight spotLights[MAX_LIGHTS];

// Calculate how a point light affects the fragment
vec3 calculatePointLight(PointLight light) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);
    float lightDist = distance(fragPos, light.pos);

    // vec3 ambient = ambientStrength * light.color;

    float diff = max(dot(norm, lightDir), 0.0f) / lightDist;
    vec3 diffuse = diff * light.color;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 8.0f);
    vec3 specular = spec * specularStrength * light.color;

    // return ambient + diffuse + specular;
    return diffuse + specular;
}

vec3 calculateDirectionalLight(DirectionalLight light) {
    return vec3(0.0f);
}

vec3 calculateSpotLight(SpotLight light) {
    vec3 fragToLight = normalize(fragPos - light.pos);
    float d = dot(fragToLight, light.dir);

    if (d < light.outerCutoffAngle) {
        return vec3(0.0f);
    }

    float eps = light.cutoffAngle - light.outerCutoffAngle;
    float intensity = clamp((d - light.outerCutoffAngle) / eps, 0.0f, 1.0f);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.pos - fragPos);
    float lightDist = distance(fragPos, light.pos);

    // vec3 ambient = ambientStrength * light.color;

    float diff = max(dot(norm, lightDir), 0.0f) / lightDist;
    vec3 diffuse = diff * intensity * light.color;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 8.0f);
    vec3 specular = spec * intensity * specularStrength * light.color;

    // return ambient + diffuse + specular;
    return diffuse + specular;
}

// Returns a color on how lighting affects the fragment
vec3 calculateLighting()
{
    vec3 color = vec3(0.0f);

    int _numPointLights = clamp(0, MAX_LIGHTS, numPointLights);
    int _numDirectionalLights = clamp(0, MAX_LIGHTS, numDirectionalLights);
    int _numSpotLights = clamp(0, MAX_LIGHTS, numSpotLights);

    // Add casters influence
    int i;
    for (i = 0; i < _numPointLights; ++i) {
        PointLight light = pointLights[i];
        if (light.enabled == 0) continue;

        color += calculatePointLight(light);
    }

    for (i = 0; i < _numDirectionalLights; ++i) {
        DirectionalLight light = directionalLights[i];
        if (light.enabled == 0) continue;

        color += calculateDirectionalLight(light);
    }

    for (i = 0; i < _numSpotLights; ++i) {
        SpotLight light = spotLights[i];
        if (light.enabled == 0) continue;

        color += calculateSpotLight(light);
    }

    // Add ambient lighting influence
    color += ambientStrength * vec3(1.0f);

    return color;
}

// Returns the mixed color for all textures
vec4 calculateTexture()
{
    if (numTextures == 0)
        return vec4(0.0f);

    // Step for mixing colors
    float mixStep = 1.0f / float(numTextures);
    vec4 color = vec4(1.0f);

    // NOTE: Can't use a for-loop because an array of samplers can only be
    //       indexed by a constant value, for example, a literal int like below

    // Mix textures based on number of binded textures
    if (numTextures > 0)
        color = mix(color, texture(textures[0], texCoord), mixStep);

    if (numTextures > 1)
        color = mix(color, texture(textures[1], texCoord), mixStep);

    if (numTextures > 2)
        color = mix(color, texture(textures[2], texCoord), mixStep);

    if (numTextures > 3)
        color = mix(color, texture(textures[3], texCoord), mixStep);

    return color;
}

void main()
{
    vec3 lighting = calculateLighting();
    vec4 texColor = calculateTexture();

    // If texture alpha is not 100%, starting mixing with mesh color
    if (texColor.a < 1.0f) {
        texColor = vec4(mix(texColor.rgb, meshColor.rgb, 1.0f - texColor.a), 1.0f);
    }

    // Final result
    FragColor = texColor * vec4(lighting, 1.0f);
}
