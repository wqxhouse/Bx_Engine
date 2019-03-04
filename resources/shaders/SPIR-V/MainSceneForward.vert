#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 0) out vec3 posView;
layout (location = 1) out vec3 normalView;
layout (location = 2) out vec2 fragtexCoord;
layout (location = 3) out mat4 fragViewMat;

layout (binding = 0) uniform Trans
{
	mat4 worldMat;
	mat4 viewMat;
	mat4 projMat;
	mat4 wvpMat;
} m_trans;

void main()
{
    mat4 vpMat = m_trans.viewMat * m_trans.worldMat;

    posView      = (vpMat * vec4(pos, 1.0f)).xyz;
	normalView   = (vpMat * vec4(normal, 0.0f)).xyz;
	fragtexCoord = texCoord;
    fragViewMat  = m_trans.viewMat;

	gl_Position  = m_trans.wvpMat * vec4(pos, 1.0f);
}