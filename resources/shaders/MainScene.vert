#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform trans
{
	mat4 world;
	mat4 view;
	mat4 proj;
	mat4 wvp;
};

uniform mat4 lightTransWVP;

out vec3 posWorld;
out vec3 normalWorld;
out vec2 fragTexCoord;

out vec3 posLightProj;

void main()
{
	posWorld     = (world * vec4(position, 1.0f)).xyz;
	normalWorld  = (world * vec4(normal, 1.0f)).xyz;
	fragTexCoord = texCoord;
	
	posLightProj = (lightTransWVP * vec4(position, 1.0f)).xyz;
	
    gl_Position  = wvp * vec4(position, 1.0f);
}