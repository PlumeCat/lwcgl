#version 400

in vec2 iPos;
in vec2 iTex;
in vec4 iCol;

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
