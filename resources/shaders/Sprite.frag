/*
	Sprite.frag
*/

#version 440 core

uniform sampler2D spriteTex;

in vec2 fragUV;

out vec4 outColor;

void main()
{
	outColor = texture(spriteTex, fragUV);
}

// End of Sprite.frag