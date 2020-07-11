#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 UV;

struct Material {
    // 纹理采集器
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emit;
    float shininess;
};

struct Light {
    vec3 position;
    
    vec3 direction;
    float inner;
    float outer;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = vec3(texture(material.diffuse, UV)) * light.ambient;
    
    vec3 normal = normalize(Normal);
    
    float lightDis = length(light.position - Position);
    vec3 lightDir = normalize(light.position - Position);
    
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta - light.outer) / (light.inner - light.outer), 0.0f, 1.0f);
    
    float attenuation = 1.0f / (light.constant + light.linear * lightDis + light.quadratic * lightDis * lightDis);
    
    vec3 diffuse = max(dot(normal, lightDir), 0.0f) * vec3(texture(material.diffuse, UV)) * light.diffuse * attenuation * intensity;
    
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    vec3 specular = spec * vec3(texture(material.specular, UV)) * light.specular * attenuation * intensity;
    
    vec3 result = specular + diffuse + ambient;
    FragColor = vec4(result, 1.0f);
}
