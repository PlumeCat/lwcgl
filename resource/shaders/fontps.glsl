#version 400

in vec2 vTex;
in vec4 vCol;
out vec4 oCol;

uniform sampler2D diffuseMap;

void main()
{
    vec4 samp = texture(diffuseMap, vTex * vec2(1, -1) + vec2(0, 1));

    if (samp.r < 0.5)
        discard;

    oCol = samp * vCol;
}
