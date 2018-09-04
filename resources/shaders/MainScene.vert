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

// uniform mat4 lightTransWVP;

out vec4 posWorldVec4;
out vec3 normalWorld;
out vec2 fragTexCoord;

// out vec4 posLightProj;

out vec3 lightProbeSampler;

void main()
{
	posWorldVec4 = (m_trans.world * vec4(position, 1.0f));
	normalWorld  = (m_trans.world * vec4(normal, 0.0f)).xyz;
	fragTexCoord = texCoord;
	
	// posLightProj = lightTransWVP * vec4(position, 1.0f);
    
    lightProbeSampler = position;//vec3(position.x, position.y, position.z);
    
    gl_Position  = m_trans.wvp * vec4(position, 1.0f);
}