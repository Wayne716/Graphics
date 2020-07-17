#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texMap;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;
uniform bool gamma;

vec3 Blinn(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfWay = normalize(lightDir + viewDir);

    vec3 diffuse = max(dot(lightDir, normal), 0) * lightColor;

    vec3 specular = lightColor;

    specular *= pow(max(dot(halfWay, normal), 0), 64.0);
    
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (gamma? distance * distance : distance);
    
    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

void main()
{
    vec3 color = texture(texMap, fs_in.TexCoords).rgb;
    vec3 lighting = vec3(0.0);
    for (int i=0; i<4; ++i)
        lighting += Blinn(normalize(fs_in.Normal), fs_in.FragPos, lightPositions[i], lightColors[i]);
    color *= lighting;
    if (gamma) color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
