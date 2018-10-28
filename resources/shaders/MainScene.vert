/*
    MainScene.vert
*/

#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

struct Trans
{
    mat4 world;
    mat4 view;
    mat4 proj;
    mat4 wvp;
};

uniform transUniformBlock
{
    Trans m_trans;
};

out vec4 posWorldVec4;
out vec3 normalWorld;
out vec2 fragTexCoord;

out vec3 lightProbeSampler;

void main()
{
	posWorldVec4 = (m_trans.world * vec4(position, 1.0f));
	normalWorld  = (m_trans.world * vec4(normal, 0.0f)).xyz;
	fragTexCoord = texCoord;

    lightProbeSampler = position;

    gl_Position  = m_trans.wvp * vec4(position, 1.0f);
}