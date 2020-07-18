#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
    for (int face=0; face<6; ++face) {
        gl_Layer = face;
        for (int i=0; i<3; ++i) {
            FragPos = gl_in[i].gl_Position;
            // 投影矩阵对每个面都是一样的
            // 视图矩阵依次对右、左、上、下、近、远计算
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
