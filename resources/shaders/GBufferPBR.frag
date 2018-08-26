/*
    GBufferPBR.frag
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
	CookTorranceMaterial m_Material;
};

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

layout(location = 0) out vec3 posWorldTexture;
layout(location = 1) out vec3 normalWorldTexture;
layout(location = 2) out vec3 texCoordTexture;
layout(location = 3) out vec3 albedoTexture;
layout(location = 4) out vec3 specularTexture;

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
    
    posWorldTexture    = posWorld;
    normalWorldTexture = normalWorld;
    texCoordTexture    = vec3(fragTexCoord, shadowAttenuation);
    albedoTexture      = m_Material.albedo;
    specularTexture    = vec3(m_Material.roughness, m_Material.metallic, m_Material.fresnel);
}
// End GBufferPBR.frag