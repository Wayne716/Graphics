#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 UV;

uniform vec3 viewPos;

struct Material {
    // 纹理采集器
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct DirectLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectLight directLight;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define LIGHTS_AMOUNT 4
uniform PointLight pointLights[LIGHTS_AMOUNT];

struct SpotLight {
    vec3 position;
    vec3 direction;
    float inner;
    float outer;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;

vec3 calDirect(DirectLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient = vec3(texture(material.diffuse, UV)) * light.ambient;
    vec3 diffuse = max(dot(normal, lightDir), 0) * vec3(texture(material.diffuse, UV)) * light.diffuse;
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0), material.shininess) * vec3(texture(material.specular, UV)) * light.specular;
    return ambient + diffuse + specular;
}

vec3 calPoint(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float lightDis = length(light.position - fragPos);
    float attenuation = 1 / (light.constant + light.linear * lightDis + light.quadratic * lightDis * lightDis);
    
    vec3 ambient = vec3(texture(material.diffuse, UV)) * light.ambient * attenuation;

    vec3 diffuse = max(dot(lightDir, normal), 0) * vec3(texture(material.diffuse, UV)) * light.diffuse * attenuation;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = max(0, dot(reflectDir, viewDir)) * vec3(texture(material.specular, UV)) * light.specular * attenuation;
    
    return ambient + diffuse + specular;
}

vec3 calSpot(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float lightDis = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * lightDis + light.quadratic * lightDis * lightDis);
    
    vec3 ambient = vec3(texture(material.diffuse, UV)) * light.ambient;
    
    vec3 diffuse = max(dot(normal, lightDir), 0.0f) * vec3(texture(material.diffuse, UV)) * light.diffuse;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess) * vec3(texture(material.specular, UV)) * light.specular;
    
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta - light.outer) / (light.inner - light.outer), 0.0f, 1.0f);
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return ambient + diffuse + specular;
}

void main()
{
    vec3 res;
    
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - Position);
    
    res += calDirect(directLight, normal, viewDir);
    
    for (int i=0; i<LIGHTS_AMOUNT; ++i)
        res += calPoint(pointLights[i], normal, Position, viewDir);
    
    res += calSpot(spotLight, normal, Position, viewDir);
    
    FragColor = vec4(res, 1.0);
}
