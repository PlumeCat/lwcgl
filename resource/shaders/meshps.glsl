// blockps.glsl

#version 400

in vec2 vTex;
in vec4 vCol;
out vec4 oCol;

uniform sampler2D diffuseMap;

void main()
{
    vec4 tex = texture(diffuseMap, vTex);
    oCol = vCol;
    // oCol *= tex;
}
