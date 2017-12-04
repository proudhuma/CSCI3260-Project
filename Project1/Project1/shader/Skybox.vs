#version 440

in layout(location = 0) vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 M;

out float visibility;

const float FogDensity = 0.05;
const float FogGradient = 3.5;

void main()
{
	float distance = length(view * M * vec4(position, 1.0));
	visibility = exp(-pow((distance * FogDensity), FogGradient));
	visibility = clamp(visibility, 0, 1);
	vec4 pos = projection * view * M * vec4(position, 1.0);
	gl_Position = pos;

	TexCoords = position;
}