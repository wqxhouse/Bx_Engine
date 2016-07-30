#version 440 core

out vec4 gl_Color;

in vec3 fragColor;

void main()
{
	gl_Color = vec4(fragColor.xyz, 1.0f);//vec4(1.0f, 0.0f, 0.0f,1.0f);
}