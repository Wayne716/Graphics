#version 330 core

in vec2 TexCoords;

out float FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

const vec2 noiseScale = vec2(800.0 * 2.0 / 4.0, 600.0 * 2.0 / 4.0);

uniform mat4 projection;

void main()
{
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 rotation = normalize(texture(texNoise, TexCoords * noiseScale).rgb);
    vec3 tangent = normalize(rotation - dot(rotation, normal) * normal);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    for (int i=0; i<kernelSize; ++i) {
        // world to view
        vec3 pos = TBN * samples[i];
        pos = fragPos + pos * radius;
        
        // view to clip
        vec4 offset = projection * vec4(pos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= pos.z + bias ? rangeCheck : 0.0);
    }
    occlusion = 1.0 - occlusion / kernelSize;
    FragColor = occlusion;
}
