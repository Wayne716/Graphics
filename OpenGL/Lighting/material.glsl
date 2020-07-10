#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// 光源对三种光照属性有不同的强度
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = material.ambient * light.ambient;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - Position);
    vec3 diffuse = max(dot(normal, lightDir), 0.0f) * material.diffuse * light.diffuse;
    
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = spec * material.specular * light.specular;
    
    vec3 result = specular + diffuse + ambient;
    FragColor = vec4(result, 1.0f);
}
