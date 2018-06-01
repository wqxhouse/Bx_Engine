#version 440 core

layout (std140) uniform material
{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	vec4 ns;
};

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

layout(location = 0) out vec4 posWorldTexture;
layout(location = 1) out vec4 normalWorldTexture;
layout(location = 2) out vec4 texCoordTexture;
layout(location = 3) out vec4 kaMaterialTexture;
layout(location = 4) out vec4 kdMaterialTexture;

void main()
{
    posWorldTexture    = vec4(posWorld, ks.x);
    normalWorldTexture = vec4(normalWorld, ks.y);
    texCoordTexture    = vec4(fragTexCoord, ks.z, ns.w);
    kaMaterialTexture  = vec4(ka, 1.0f);
    kdMaterialTexture  = vec4(kd, 1.0f);
}