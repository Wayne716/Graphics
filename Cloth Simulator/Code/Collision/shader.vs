#version 330 corelayout (location = 0) in vec3 pos;layout (location = 1) in vec3 nor;layout (location = 2) in vec2 tex;out vec3 Position;out vec3 Normal;out vec2 TexCoords;uniform mat4 projection;uniform mat4 view;uniform mat4 model;void main(){    Position = pos;    Normal = nor;    TexCoords = tex;        gl_Position = projection * view * model * vec4(pos, 1.0);}