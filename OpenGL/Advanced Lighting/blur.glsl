#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D image;
uniform bool horizontal;
uniform float weights[5] = float[] (
                                    0.2270270270,
                                    0.1945945946,
                                    0.1216216216,
                                    0.0540540541,
                                    0.0162162162);

void main()
{
    vec2 texel = 1.0 / textureSize(image, 0);
    vec3 res = texture(image, TexCoords).rgb * weights[0];
    if (horizontal) {
        for (int i=1; i<5 ; ++i) {
            res += texture(image, TexCoords + vec2(texel.x * i, 0.0f)).rgb * weights[i];
            res += texture(image, TexCoords - vec2(texel.x * i, 0.0f)).rgb * weights[i];
        }
    }
    else {
        for (int i=1; i<5; ++i) {
            res += texture(image, TexCoords + vec2(0.0f, texel.y * i)).rgb * weights[i];
            res += texture(image, TexCoords - vec2(0.0f, texel.y * i)).rgb * weights[i];
        }
    }
    FragColor = vec4(res, 1.0f);
}
