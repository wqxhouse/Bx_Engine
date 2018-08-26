#version 440 core

in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 uniformColor;
uniform sampler2D sampler;

out vec4 gl_Color;

void main()
{
	gl_Color = vec4(fragNormal.xyz, 1.0f);
}