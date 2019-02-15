#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 0) out vec3 posView;
layout (location = 1) out vec3 normalView;
layout (location = 2) out vec2 fragTexCoord;

layout (binding = 0) uniform TransMat
{
    mat4 worldMat;
    mat4 viewMat;
    mat4 projMat;
	mat4 wvpMat;
} m_transMat;

void main()
{
    mat4 wvMat  = m_transMat.viewMat * m_transMat.worldMat;

    vec4 posViewHomogeneous = wvMat * vec4(pos, 1.0f);
    posView                 = posViewHomogeneous.xyz / posViewHomogeneous.w;

    vec4 normalViewHomogeneous = wvMat * vec4(normal, 1.0f);
    normalView                 = normalViewHomogeneous.xyz / normalViewHomogeneous.w;

    fragTexCoord = texCoord;

    gl_Position = m_transMat.wvpMat * vec4(pos, 1.0f);
}