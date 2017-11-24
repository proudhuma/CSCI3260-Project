#version 440

in vec3 normalWorld;
in vec2 UV;

out vec4 daColor;

uniform sampler2D myTextureSampler_0;
uniform sampler2D myTextureSampler_1;

void main()
{
	vec3 normal = normalize(normalWorld);
	// obtain normal from normal map in range [0,1]
	normal = texture(myTextureSampler_1, UV).rgb;
	// Transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);

	vec3 Material_Clr = texture( myTextureSampler_0, UV).rgb;

	daColor = vec4(Material_Clr, 1.0);
}
