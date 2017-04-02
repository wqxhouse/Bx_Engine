#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
	fragNormal = normal;
	fragTexCoord = texCoord;
	
	mat4 mvp = proj * view * world;
    gl_Position = mvp * vec4(position, 1.0f);
}