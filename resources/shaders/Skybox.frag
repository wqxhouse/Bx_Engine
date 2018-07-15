#version 440 core

samplerCube skyboxCubemap

in vec3 skyboxTextureSampler;

out vec4 outColor;

void main()
{
    outColor = textureCube(skyboxCubemap, skyboxTextureSampler);
}