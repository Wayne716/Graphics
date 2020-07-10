#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Position);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
    
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), 32);
    vec3 specular = spec * specularStrength * lightColor;
    
    vec3 result = (specular + diffuse + ambient) * objectColor;
    FragColor = vec4(result, 1.0);
}
