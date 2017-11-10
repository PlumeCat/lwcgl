// blockps.glsl

#version 400

in vec2 vTex;
in vec4 vCol;

uniform sampler2D diffuseMap;

void main()
{
    vec4 tex = texture(diffuseMap, vTex);
    gl_FragColor = vCol * tex;
}