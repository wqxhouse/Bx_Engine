/*
    Skybox.frag
*/

#version 440 core

uniform samplerCube skyboxCubemap;

in vec3 skyboxTextureSampler;

out vec4 outColor;

void main()
{
    outColor = texture(skyboxCubemap, skyboxTextureSampler);
}

// End of Skybox.frag