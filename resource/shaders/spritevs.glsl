// spritevs.glsl
#version 400
#vertex sprite_vertex

out vec2 vTex;
out vec4 vCol;

uniform mat4 View;
uniform mat4 Proj;

void main()
{
    vec4 pos = Proj * View * vec4(iPos, 1, 1);

    vTex = iTex;
    vCol = iCol;
    gl_Position = pos;
}
