#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

/*uniform trans
{
	mat4 world;
	mat4 view;
	mat4 proj;
};*/
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 rot;

out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
	mat4 mvp = proj * view * world;
	
	fragTexCoord = texCoord;
	fragNormal = normalize(rot * vec4(normal, 1.0f)).xyz;
	
    gl_Position = mvp * vec4(position, 1.0f);
}