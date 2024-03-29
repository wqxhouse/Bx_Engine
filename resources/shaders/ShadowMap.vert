/*
    ShadowMap.vert
*/

#version 440 core

layout (location = 0) in vec3 position;

uniform mat4 wvp;

void main()
{
	gl_Position = wvp * vec4(position, 1.0f);
}

// End of ShadowMap.vert