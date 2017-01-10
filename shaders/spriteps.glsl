#version 400

in vec4 vCol;
in vec2 vTex;
out vec4 fragColor;

uniform sampler2D texture;

void main()
{
	fragColor = texture2D(texture, vTex) * vCol;
	//fragColor = vec4(vTex.xy, 1, 1);
}
