#version 400

in vec2 vTex;
in vec4 vCol;
out vec4 fragColor;

uniform sampler2D texture;

void main()
{
	fragColor = vCol;
	// fragColor = vec4(vTex.xy, 1, 1);
}
