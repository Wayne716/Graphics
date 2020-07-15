#version 330 core

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;

void main()
{
    vec3 viewDir = normalize(Position - cameraPos);
    vec3 reflectDir = normalize(reflect(viewDir, normalize(Normal)));
    vec3 reflection = vec3(texture(cubeMap, reflectDir));
    
    FragColor = vec4(reflection, 1.0);
}
