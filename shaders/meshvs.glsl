#version 400

in vec3 iPos;
in vec3 iNorm;
in vec2 iTex;
out vec2 vTex;
out vec4 vCol;

uniform mat4 view;
uniform mat4 world;
uniform mat4 proj;

void main()
{
	vec3 lightDir = vec3(-1, -2, -2);
	float d = dot(normalize(iNorm), -normalize(lightDir));

	vTex = iTex;
	vCol = vec4(d,d,d,1);
	gl_Position = proj * view * world * vec4(iPos, 1);
}
