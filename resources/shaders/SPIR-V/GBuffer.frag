#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_MESH_NUM    256

layout (location = 0) in vec3 posView;
layout (location = 1) in vec3 normalView;
layout (location = 2) in vec2 fragTexCoord;

layout (location = 0) out vec3 gBufferPosViewTexture;
layout (location = 1) out vec3 gBufferNormalViewTexture;
layout (location = 2) out vec4 gBufferAlbedoTexture;
layout (location = 3) out vec4 gBufferSpecularTexture;

layout (binding = 1) uniform MaterialUbo
{
	vec4  materialIndex;
    float ns; // vec3 paddingNS;
	vec4  padding[14];
};

layout (binding = 2) uniform sampler2D AlbedoTexture[MAX_MESH_NUM];
layout (binding = 3) uniform sampler2D SpecularTexture[MAX_MESH_NUM];

void main()
{
    gBufferPosViewTexture    = posView;
    gBufferNormalViewTexture = normalView;

    const uint albedoTextureIndex = floatBitsToUint(materialIndex.x);
    const uint specTextureIndex   = floatBitsToUint(materialIndex.y);
    
    gBufferAlbedoTexture          = texture(AlbedoTexture[albedoTextureIndex], fragTexCoord);
    gBufferSpecularTexture        = vec4(texture(SpecularTexture[specTextureIndex], fragTexCoord).xyz, ns);
}