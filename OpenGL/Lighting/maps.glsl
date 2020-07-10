#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 UV;

struct Material {
    // 纹理采集器
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

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
    vec3 ambient = vec3(texture(material.diffuse, UV)) * light.ambient;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - Position);
    vec3 diffuse = max(dot(normal, lightDir), 0.0f) * vec3(texture(material.diffuse, UV)) * light.diffuse;
    
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    // 根据纹理的颜色亮度定义镜面反射强度
    vec3 specular = spec * vec3(texture(material.specular, UV)) * light.specular;
    
    // 边框上的颜色强度增加了
    vec3 result = specular + diffuse + ambient;
    FragColor = vec4(result, 1.0f);
}
