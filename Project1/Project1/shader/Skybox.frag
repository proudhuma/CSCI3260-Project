#version 440

in vec3 TexCoords;
in float visibility;
out vec4 daColor;

uniform samplerCube skybox;

void main()
{
	vec3 Fog_Color = vec3(0.5, 0.5, 0.5);

	vec4 calColor = texture(skybox, TexCoords);

	vec4 Fog_Real_Color = vec4(Fog_Color, 1.0);

	daColor = mix(Fog_Real_Color, calColor, 0.2);

	//daColor = texture(skybox, TexCoords);
}
