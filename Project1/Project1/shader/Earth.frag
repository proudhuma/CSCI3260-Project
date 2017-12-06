#version 440

in vec3 normalWorld;
in vec2 UV;
in vec3 vertexPositionWorld;
in float visibility;
out vec4 daColor;

uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

uniform sampler2D myTextureSampler_0;
uniform sampler2D myTextureSampler_1;

void main()
{


	//diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	
	//specular
	//calculate reflect light direction
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	//calculate direction from eye to object
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	//calculate light brightness according to the angle between eye and reflect light
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	//control lobe of specularLight
	s = pow(s, 30);

	vec3 SpecularLightColor = vec3(0.6,0.6,0.6);

	vec3 normal = normalize(normalWorld);
	// obtain normal from normal map in range [0,1]
	normal = texture(myTextureSampler_1, UV).rgb;
	// Transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);

	vec3 MaterialAmbientColor = texture(myTextureSampler_0, UV).rgb;
	vec3 MaterialDiffuseColor = texture(myTextureSampler_0, UV).rgb;
	vec3 MaterialSpecularColor = vec3(0.6, 0.6, 0.6);

	vec3 color=
	// Ambient : simulates indirect lighting
	MaterialAmbientColor* ambientLight+
	// Diffuse : "color" of the object
	MaterialDiffuseColor * brightness * 2.0 +
	// Specular : reflective highlight, like a mirror
	MaterialSpecularColor * SpecularLightColor * s;

	vec4 calColor = vec4(color, 1.0);
	vec3 Fog_Color = vec3(0.5, 0.5, 0.5);
	vec4 Fog_Real_Color = vec4(Fog_Color, 1.0);
	daColor = mix(Fog_Real_Color, calColor, visibility);
}
