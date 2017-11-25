#version 440 //GLSL version your computer supports

uniform vec3 ambientLight;
uniform vec3 lightPositionWorld;

out vec4 daColor;
in vec3 theColor;

void main()
{
	daColor = vec4(theColor,1.0) ;
}