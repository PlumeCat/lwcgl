#version 400

in vec2 vTex;
in vec3 vNorm;
in vec4 vCol;
out vec4 fragColor;

uniform samplerCube cubemap;

void main()
{
	fragColor = texture(cubemap, normalize(vNorm));
	// fragColor = vec4(vTex.xy, 1, 1);
}
