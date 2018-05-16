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

out vec3 posWorld;
out vec3 normalWorld;
out vec2 fragTexCoord;

void main()
{
	posWorld     = (world * vec4(position, 1.0f)).xyz;
	normalWorld  = (world * vec4(normal, 1.0f)).xyz;
	fragTexCoord = texCoord;
	
    gl_Position  = wvp * vec4(position, 1.0f);
}