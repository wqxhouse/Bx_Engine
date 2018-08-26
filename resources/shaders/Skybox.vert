/*
    Skybox.vert
*/

#version 440 core

layout (location = 0) in vec3 position;

uniform mat4 wvp;

out vec3 skyboxTextureSampler;

void main()
{
    skyboxTextureSampler = position;
    
    vec4 fragPos = wvp * vec4(position, 1.0f);
    gl_Position = fragPos.xyww; // Set z to w for keep the z to 1.0f, which is the far clip
}

// End of Skybox.vert