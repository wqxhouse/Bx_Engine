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

uniform vec3 cameraPos;

in vec3 posWorld;
in vec3 normalWorld;

in vec3 posView;
in vec3 normalView;

in vec2 fragTexCoord;

in vec4 posLightProj;

layout(location = 0) out vec4 posViewTexture;
layout(location = 1) out vec3 normalViewTexture;
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
		shadowAttenuation = 0.05f;
	}
	
	return shadowAttenuation;
}

void main()
{    
    float shadowAttenuation = castingShadow();

    posViewTexture     = vec4(posView, 1.0f);
    normalViewTexture  = normalView;
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

            // Store the world space reflection vector for sampling light probe
            vec3 view       = normalize(cameraPos - posWorld);
            vec3 normal     = normalize(normalWorld);

            vec3 reflection = normalize(2 * dot(normal, view) * normal - view);

            environmentLightTexture = normal;//reflection;
        }
        default:
            break;
    }
}