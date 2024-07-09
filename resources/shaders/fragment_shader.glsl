#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D texture0;
uniform vec4 color;

const vec3 lightPos = vec3(0.0f, 3.0f, 0.0f);
const vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

const float ambientStrength = 0.1f;

vec3 calculateLighting()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float lightDist = distance(fragPos, lightPos);

    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(norm, lightDir), 0.0f) / pow(lightDist, 0.5f);
    vec3 diffuse = diff * lightColor;

    return ambient + diffuse;
}

void main()
{
    vec3 lighting = calculateLighting();
    FragColor = texture(texture0, texCoord) * color * vec4(lighting, 1.0f);
}
