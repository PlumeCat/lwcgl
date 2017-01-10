#version 400

in vec3 iPos;
in vec3 iNorm;
in vec2 iTex;
out vec2 vTex;
out vec4 vCol;

uniform mat4 playerOffset;
uniform mat4 view;
// uniform mat4 world;
uniform mat4 proj;

uniform mat4 worlds[1024];

void main()
{
	vec3 lightDir = vec3(-1, -2, -2);
	float d = dot(normalize(iNorm), -normalize(lightDir));

	mat4 world = worlds[gl_InstanceID];

	vTex = iTex;
	vCol = vec4(d,d,d,1);
	gl_Position = proj * view * playerOffset * world * vec4(iPos, 1);
}
