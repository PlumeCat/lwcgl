// cubevs.glsl
#version 400
#vertex mesh_vertex

out vec2 vTex;
out vec4 vCol;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;
uniform vec4 Colour;

void main()
{
    vec3 lightDir = vec3(-1, -3, 2);
    vec3 worldNorm = (World * vec4(iNorm, 0)).xyz;
    float d = dot(normalize(-lightDir), normalize(worldNorm));
    vec4 pos = Proj * View * World * vec4(iPos, 1);

    vTex = iTex;
    vCol = Colour * vec4(d,d,d,1);
    gl_Position = pos;
}
