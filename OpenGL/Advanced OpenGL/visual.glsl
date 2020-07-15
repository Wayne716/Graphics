#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in vec3[] norm;

uniform mat4 projection;

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    
    gl_Position = projection * (gl_in[index].gl_Position + vec4(norm[index], 0.0) * MAGNITUDE);
    EmitVertex();
    
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}
