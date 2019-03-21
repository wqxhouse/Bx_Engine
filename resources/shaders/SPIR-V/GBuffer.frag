#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_MESH_NUM    256

layout (location = 0) in vec3 posView;
layout (location = 1) in vec3 normalView;
layout (location = 2) in vec2 fragTexCoord;

layout (location = 0) out vec3 gBufferPosViewTexture;
layout (location = 1) out vec3 gBufferNormalViewTexture;
layout (location = 2) out vec4 gbufferAlbedoTexture;

layout (binding = 1) uniform MaterialUbo
{
	vec4 materialIndex;
	vec4 padding[15];
};

layout (binding = 2) uniform sampler2D AlbedoTexture[MAX_MESH_NUM];

// TODO
// layout (locatoin = 3) out vec3 specularTexture;

void main()
{
    gBufferPosViewTexture    = posView;
    gBufferNormalViewTexture = normalView;

    const uint albedoTextureIndex = floatBitsToUint(materialIndex.x);
    gbufferAlbedoTexture     = texture(AlbedoTexture[albedoTextureIndex], fragTexCoord);
}