#version 440  // GLSL version your computer supports

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

out vec3 theColor;

uniform mat4 PM;
uniform mat4 MM;
uniform mat4 VM;

void main()
{
	vec4 v = vec4(position, 1.0);
	gl_Position = PM * VM * MM * v;
	theColor = vertexColor;

}