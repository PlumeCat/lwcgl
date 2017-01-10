#version 400

in vec3 iPos;
in vec4 iCol;
in vec2 iTex;
out vec4 vCol;
out vec2 vTex;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	vCol = iCol;
	vTex = iTex;
	gl_Position = proj * view * vec4(iPos, 1);
}
