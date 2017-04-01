#version 440 core

out vec4 gl_Color;

in vec3 fragColor;

uniform vec3 uniformColor;

void main()
{
	vec3 color3 = fragColor * uniformColor;
	gl_Color = vec4(color3.xyz, 1.0f);
}