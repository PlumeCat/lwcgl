#version 400

in vec2 vTex;
in vec4 vCol;
out vec4 oCol;

uniform sampler2D diffuseMap;

void main()
{
    vec4 samp = texture(diffuseMap, vTex);
    if (samp.r < 0.075)
        discard;
    oCol = vCol * samp * 2;
}
