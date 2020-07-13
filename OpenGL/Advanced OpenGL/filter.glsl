#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
    vec2 offsets[9] = vec2[](vec2(-offset, offset),
                             vec2(0.0, offset),
                             vec2(offset, offset),
                             vec2(-offset, 0.0),
                             vec2(0.0, 0.0),
                             vec2(offset, 0.0),
                             vec2(-offset, -offset),
                             vec2(0.0, -offset),
                             vec2(offset, -offset));
    
    // 卷积矩阵的元素和为1 否则影响明暗
    
    // 锐化
    float kernel1[9] = float[](-2, -2, -2,
                               -2,  16, -2,
                               -2, -2, -2);
    
    // 模糊
    float kernel2[9] = float[](1.0/16, 2.0/16, 1.0/16,
                               2.0/16, 4.0/16, 2.0/16,
                               1.0/16, 2.0/16, 1.0/16);
                              
    
    // 边缘检测
    float kernel3[9] = float[](1,  1, 1,
                               1, -8, 1,
                               1,  1, 1);
    
    // 采样周围点的纹理结果
    vec3 sampleTex[9];
    for (int i=0; i<9; ++i)
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    
    // 卷积运算
    vec3 col = vec3(0.0f);
    for (int i=0; i<9; ++i)
        col += sampleTex[i] * kernel1[i];
    
    FragColor = vec4(col, 1.0);
}
