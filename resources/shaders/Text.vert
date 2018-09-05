/*
    Text.vert
*/

#version 440 core

#include <Utilities.hglsl>

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform resolutionUniformBlock
{
    Resolution resolution;
};

out vec2 fragUV;

void main()
{
    vec2 pos = (position - vec2(0.5f)) * 2.0f;
    fragUV = uv;
    
    gl_Position = vec4(pos, 0.0f, 1.0f);  
}

// End of Text.vert