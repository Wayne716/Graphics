#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D textureMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

void main()
{
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfWay = normalize(lightDir + viewDir);
    
    // parallax mapping
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float layers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    
    float layerDepth = 1.0 / layers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * 0.1f;
    vec2 deltaTex = P / layers;
    vec2 tex = fs_in.TexCoords;
    float depthValue = texture(depthMap, tex).r;
    
    while (currentLayerDepth < depthValue) {
        tex -= deltaTex;
        currentLayerDepth += layerDepth;
        depthValue = texture(depthMap, tex).r;
    }
    
    vec2 prevTex = tex + deltaTex;
    float afterDepth = depthValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTex).r - currentLayerDepth + layerDepth;
    float weight =  afterDepth / (afterDepth - beforeDepth);
    tex = weight * prevTex + (1 - weight) * tex;
    
    if (tex.x > 1.0 || tex.x < 0.0 || tex.y > 1.0 || tex.y < 0.0)
        discard;
    
    // normal mapping
    vec3 normal = texture(normalMap, tex).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    vec3 color = texture(textureMap, tex).rgb;
    
    vec3 ambient = 0.1 * color;
    vec3 diffuse = max(dot(lightDir, normal), 0.0)  * color;
    vec3 specular = pow(max(dot(normal, halfWay), 0.0), 32.0) * vec3(5.0f);
    
    color = pow(ambient + diffuse + specular, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
