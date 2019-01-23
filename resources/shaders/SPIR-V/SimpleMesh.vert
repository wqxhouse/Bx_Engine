#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 0) out vec4 fragNormal;
layout (location = 1) out vec2 fragtexCoord;

layout (binding = 0) uniform Trans
{
	mat4 wvp;
} m_trans;

void main()
{
	gl_Position  = m_trans.wvp * vec4(pos, 1.0f);
	fragNormal   = vec4(normal, 1.0f);
	fragtexCoord = texCoord;
}