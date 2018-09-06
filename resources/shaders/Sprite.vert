/*
	Sprite.vert
*/

#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 fragUV;

void main()
{
	vec3 pos = (position - 0.5f) * 2.0f;
	fragUV = uv;
	
	gl_Position = vec4(pos, 1.0f);
}

// End of Sprite.vert