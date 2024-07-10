#version 330 core
out vec4 FragColor;

// Vertex info
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

// Entity info
uniform vec4 meshColor;

// Entity textures
const int MAX_TEXTURES = 4;
uniform int numTextures;
uniform sampler2D textures[MAX_TEXTURES];

// General info
uniform vec3 viewPos;

// Lighting info
uniform vec3 lightPos;
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

uniform float ambientStrength = 0.1f;
uniform float lightStrength = 1.f;
uniform float specularStrength = 0.5f;

// Returns a color on how lighting affects the fragment
vec3 calculateLighting()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float lightDist = distance(fragPos, lightPos);

    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0f) / lightDist;
    vec3 diffuse = diff * lightStrength * lightColor;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = specularStrength * spec * lightColor;

    return ambient + diffuse + specular;
}

// Returns the mixed color for all textures
vec4 calculateTexture()
{
    float mixStep = 1.0f / float(numTextures);
    vec4 color = vec4(1.0f);

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
    if (texColor.a < 1.0f) {
        texColor = vec4(mix(texColor.rgb, meshColor.rgb, 1.0f - texColor.a), 1.0f);
    }
    FragColor = texColor * meshColor * vec4(lighting, 1.0f);
}
