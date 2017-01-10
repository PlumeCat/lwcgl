#version 400

in vec2 vTex;
in vec4 vCol;
out vec4 fragColor;

uniform sampler2D texture;
uniform vec4 emissive;

void main()
{
	fragColor = vCol + emissive;
	// fragColor = vec4(vTex.xy, 1, 1);
}
