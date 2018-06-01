#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 worldMatrix;
uniform mat4 wvp;

out vec3 posWorld;
out vec3 normalWorld;
out vec2 fragTexCoord;

void main()
{
    posWorld     = (worldMatrix * vec4(position, 1.0f)).xyz;
    normalWorld  = (worldMatrix * vec4(normal, 0.0f)).xyz;
    fragTexCoord = texCoord;
    gl_Position  = wvp * vec4(position, 1.0f);
}
