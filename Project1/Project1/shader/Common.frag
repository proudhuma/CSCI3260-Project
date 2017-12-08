#version 440

in vec2 UV;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

in float visibility;

out vec4 daColor;

uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform vec3 fogcolor;
uniform int fogON;
uniform sampler2D myTextureSampler;

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
	//s *= specularLightControl;
	//control lobe of specularLight
	s = pow(s, 30);

	vec3 SpecularLightColor = vec3(0.6,0.6,0.6);

	vec3 MaterialAmbientColor = texture(myTextureSampler, UV).rgb;
	vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;
	vec3 MaterialSpecularColor = vec3(0.6, 0.6, 0.6);

	vec3 color=
	// Ambient : simulates indirect lighting
	MaterialAmbientColor* ambientLight+
	// Diffuse : "color" of the object
	MaterialDiffuseColor * brightness * 2.0 +
	// Specular : reflective highlight, like a mirror
	MaterialSpecularColor * SpecularLightColor * s;

	//vec3 Material_Clr = texture( myTextureSampler, UV).rgb;

	//vec3 Fog_Color = vec3(0.5, 0.5, 0.5);
	vec3 Fog_Color = fogcolor;

	vec4 calColor = vec4(color, 1.0);

	vec4 Fog_Real_Color = vec4(Fog_Color, 1.0);

	if(fogON == 1)
		daColor = mix(Fog_Real_Color, calColor, visibility);
	else
		daColor = mix(Fog_Real_Color, calColor, 1.0);
}
