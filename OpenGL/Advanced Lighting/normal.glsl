#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(pos, 1.0));
    vs_out.TexCoords = tex;
    
    mat3 fixedNormal = transpose(inverse(mat3(model)));
    vec3 T = normalize(fixedNormal * tangent);
    vec3 N = normalize(fixedNormal * normal);
    // schmidt
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    // inverse orthogonal matrices
    mat3 TBN = transpose(mat3(T, B, N));
    
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;
    
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}
