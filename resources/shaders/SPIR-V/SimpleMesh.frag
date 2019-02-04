#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout (binding = 0) uniform TestUniform
{
    vec4 color;
} m_color;

layout (binding = 1) uniform sampler2D TestTexture;

void main()
{
	outColor = vec4(texture(TestTexture, fragTexCoord).xyz, 1.0f);
}