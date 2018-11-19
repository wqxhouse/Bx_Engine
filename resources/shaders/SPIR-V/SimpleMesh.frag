#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}