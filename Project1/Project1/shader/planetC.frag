#version 440

in vec2 UV;

out vec4 daColor;

uniform sampler2D myTextureSampler_0;
uniform sampler2D myTextureSampler_1;

void main()
{
	vec3 Material_Clr = 0.3 * texture( myTextureSampler_0, UV).rgb + 0.7 * texture(myTextureSampler_1, UV).rgb;

	daColor = vec4(Material_Clr, 1.0);
}