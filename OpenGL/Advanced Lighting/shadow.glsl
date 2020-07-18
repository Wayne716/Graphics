#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 LightSpace;
} fs_in;

uniform sampler2D texMap;
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform vec3 lightPos;

vec3 Blinn(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfWay = normalize(lightDir + viewDir);

    vec3 diffuse = max(dot(lightDir, normal), 0) * lightColor;

    vec3 specular = lightColor;

    specular *= pow(max(dot(halfWay, normal), 0), 64.0);
    
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (distance * distance);
    
    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

float Shadow(vec4 lightSpace)
{
    // for perspective (orthographic w = 1)
    vec3 ndc = lightSpace.xyz / lightSpace.w;
    ndc = ndc * 0.5 + 0.5;
    // depth read from shadow map which range from 0 ~ 1
    float closestDepth = texture(shadowMap, ndc.xy).r;
    // depth of this fragment
    float currentDepth = ndc.z;
    
    // out of frustum
    if (currentDepth > 1.0f) return 0.0f;
    
    float bias = max(0.05 * (1.0 - dot(normalize(fs_in.Normal), normalize(lightPos - fs_in.FragPos))), 0.005);
        
    // percentage-closer filtering
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x=-1; x<=1; ++x) {
        for (int y=-1; y<=1; ++y) {
            float pDepth = texture(shadowMap, ndc.xy + vec2(x, y) * texelSize).r;
            shadow += ((currentDepth - bias) > pDepth) ? 1.0f : 0.0f;
        }
    }
    shadow /= 9.0f;
    return shadow;
}

void main()
{
    vec3 color = texture(texMap, fs_in.TexCoords).rgb;
    vec3 lighting = Blinn(normalize(fs_in.Normal), fs_in.FragPos, lightPos, vec3(10.0f));
    color *= lighting;
    color = (0.3 + (1 - Shadow(fs_in.LightSpace))) * color;
    
    // gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, 1.0);
}
