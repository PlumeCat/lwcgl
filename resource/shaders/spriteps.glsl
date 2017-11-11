#version 400

in vec2 vTex;
in vec4 vCol;
out vec4 oCol;

uniform sampler2D diffuseMap;

void main()
{
    oCol = vCol * texture(diffuseMap, vTex);
}
