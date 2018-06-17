/*
    GBuffer.frag
*/
#version 440 core

#include <Material.hglsl>
#include <Utilities.hglsl>

uniform sampler2D shadowMapSampler;
//uniform sampler2DMS shadowMapSampler;

layout (std140) uniform shadowMapResolutionUniformBlock
{
    ShadowMapResolution m_shadowMapResolution;
};

layout (std140) uniform gMaterial
{
	PhongMaterial m_phongMaterial;
};

layout (std140) uniform gMaterialPBR
{
	CookTorranceMaterial m_pbrMaterial;
};

uniform int materialType;

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

layout(location = 0) out vec3 posWorldTexture;
layout(location = 1) out vec3 normalWorldTexture;
layout(location = 2) out vec3 texCoordTexture;
layout(location = 3) out vec4 albedoTexture;
layout(location = 4) out vec4 specularTexture;
layout(location = 5) out vec3 environmentLightTexture;

float castingShadow()
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;
	
    // Multisampling, need integer coordinate
    //posLight.x = posLight.x * 2560.0f;//m_shadowMapResolution.width;
    //posLight.y = posLight.y * 2560.0f;//m_shadowMapResolution.height;
    
	float depth = texture(shadowMapSampler, posLight.xy).r;
    /*float depth = 0.0f;
    
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
    depth *= 0.25f;*/
	
	if (depth < posLight.z - 0.000001f)
	{
		shadowAttenuation = 0.2f;
	}
	
	return shadowAttenuation;
}

void main()
{    
    float shadowAttenuation = castingShadow();
    
    posWorldTexture    = posWorld;
    normalWorldTexture = normalWorld;
    texCoordTexture    = vec3(fragTexCoord, shadowAttenuation);

    switch (materialType)
    {
        case PHONG_MATERIAL:
        {
            albedoTexture            = vec4(m_phongMaterial.kd, 1.0f);
            specularTexture          = m_phongMaterial.ks;
            environmentLightTexture  = m_phongMaterial.ka;
            break;
        }
        case ALBEDO_MATERIAL:
        // TODO
            break;
        case COOKTORRANCE_MATERIAL:
        {
            albedoTexture           = vec4(m_pbrMaterial.albedo, 1.0f);
            specularTexture         = vec4(m_pbrMaterial.roughness, m_pbrMaterial.metallic, m_pbrMaterial.fresnel, -1.0f);

            // TODO: Calculate enviroment light
            environmentLightTexture = vec3(0.0f, 0.0f, 0.0f);
        }
        default:
            break;
    }
    
    //albedoTexture           = vec4(m_pbrMaterial.albedo, 1.0f);
    //specularTexture         = vec4(m_pbrMaterial.roughness, m_pbrMaterial.metallic, m_pbrMaterial.fresnel, 1.0f);

    // TODO: Calculate enviroment light
    //environmentLightTexture = vec3(0.0f, 0.0f, 0.0f);
}