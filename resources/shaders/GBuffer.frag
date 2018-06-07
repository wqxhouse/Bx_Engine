/*
    GBuffer.frag
*/
#version 440 core

#include <Material.hglsl>
#include <Utilities.hglsl>

uniform sampler2DMS shadowMapSampler;

layout (std140) uniform shadowMapResolutionUniformBlock
{
    ShadowMapResolution m_shadowMapResolution;
};

layout (std140) uniform gMaterial
{
	PhongMaterial m_phongMaterial;
};

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

layout(location = 0) out vec4 posWorldTexture;
layout(location = 1) out vec4 normalWorldTexture;
layout(location = 2) out vec4 texCoordTexture;
layout(location = 3) out vec4 kaMaterialTexture;
layout(location = 4) out vec4 kdMaterialTexture;

float castingShadow()
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;
	
    // Multisampling, need integer coordinate
    posLight.x = posLight.x * 2560.0f;//m_shadowMapResolution.width;
    posLight.y = posLight.y * 2560.0f;//m_shadowMapResolution.height;
    
	//float depth = texture(shadowMapSampler, posLight.xy).r;
    float depth = 0.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        float pcfDepth = 0.0f;
        for (int j = -1; j < 1; ++j)
        {
            for(int k = -1; k < 1; ++k)
            {
                float tempPcfDepth = texelFetch(shadowMapSampler, ivec2(posLight.xy) + ivec2(j, k), i).r;                
                pcfDepth += ((tempPcfDepth < posLight.z - 0.000001f) ? 0.0f : tempPcfDepth);
            }
        }
        depth += (pcfDepth * 0.111111f); // pcfDepth / 9.0f
    }
    depth *= 0.25f;
	
	if (depth < posLight.z - 0.000001f)
	{
		shadowAttenuation = 0.5f;
	}
	
	return shadowAttenuation;
}

void main()
{    
    float shadowAttenuation = castingShadow();
    
    posWorldTexture    = vec4(posWorld, m_phongMaterial.ks.x);
    normalWorldTexture = vec4(normalWorld, m_phongMaterial.ks.y);
    texCoordTexture    = vec4(fragTexCoord, m_phongMaterial.ks.z, m_phongMaterial.ks.w);
    kaMaterialTexture  = vec4(m_phongMaterial.ka, shadowAttenuation);
    kdMaterialTexture  = vec4(m_phongMaterial.kd, 1.0f);
}