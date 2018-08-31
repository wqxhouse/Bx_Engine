/*
    MainSceneCookTorrance.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Material.hglsl>
#include <Utilities.hglsl>
#include <BRDF.hglsl>

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

in vec3 lightProbeSampler;

uniform sampler2D sampler;
uniform sampler2D shadowMapSampler;
//uniform sampler2DMS shadowMapSampler;

uniform samplerCube lightProbeCubemap;

uniform shadowMapResolutionUniformBlock
{
    Resolution m_shadowMapResolution;
};

layout (std140) uniform lightArrayUniformBlock
{
    Light m_light[MAX_LIGHT_NUM];
};

layout (std140) uniform material
{
	PhongMaterial m_phongMaterial;
};

uniform vec3 eyePos;

out vec4 outColor;

float castingShadow()
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;

	float depth = texture(shadowMapSampler, posLight.xy).r;
    
    // TODO: Enable multi-sampled shadow map
    
    /* float depth = 0.0f;
	
    // Multisampling, need integer coordinate
    posLight.x = posLight.x * m_shadowMapResolution.width;
    posLight.y = posLight.y * m_shadowMapResolution.height;

    for (int i = 0; i < 4; ++i)
    {        
        depth += texelFetch(shadowMapSampler, ivec2(posLight.xy), i).r;
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
		shadowAttenuation = 0.0f;
	}
	
	return shadowAttenuation;
}

vec3 calCookTorranceRadiance(
    const vec3  view,
    const vec3  normal,
    const vec3  lightDir,
    const vec3  lightColor,
    const float shadowSpecularAttenuation)
{
    vec3 L = -lightDir;
    
    float NoL = clamp(dot(normal, L), 0.0f, 1.0f);
    
    vec3 brdf = calCookTorranceBRDF(view, normal, L, NoL, m_cookTorranceMaterial, /*shadowSpecularAttenuation*/ 1.0f);
    
    vec3 radiance = brdf * lightColor * NoL;
    
    return radiance;
}

void main()
{
	vec3 radiance;
	for (int i = 0; i < 1; ++i)
	{
		DirectionalLight m_directionalLight;
		m_directionalLight.lightBase = m_light[i].lightBase;
		m_directionalLight.dir = m_light[i].data[i].xyz;
		
        vec3 view       = normalize(eyePos - posWorld);
        vec3 normal     = normalize(normalWorld);
        vec3 dir        = normalize(m_directionalLight.dir);
        vec3 lightColor = m_directionalLight.lightBase.color;

        vec3 reflection = normalize(2 * dot(normalWorld, view) * normalWorld - view);

        // Light Probe
        // 0 - 1 roughtness; 0 - 7 mipmap
        vec3 environmentLight = texture(lightProbeCubemap, reflection, m_cookTorranceMaterial.roughness * 7.0f).xyz;

        float shadowAttenuation         = castingShadow();
        float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

        vec3 lightRadiance            = calCookTorranceRadiance(view, normal, dir, lightColor, shadowSpecularAttenuation);
        
        // Shadow casting
        lightRadiance *= shadowAttenuation;
        
        // Fetch environmentLight
        vec3 environmentLightRadiance = calCookTorranceRadiance(view, normal, -reflection, environmentLight, shadowSpecularAttenuation);

        // TODO: Indirect radiance
        // Combine the direct radiance and environmentLightRadiance
        radiance += lightRadiance + environmentLightRadiance;
    }

    // Gamma correction
    radiance = gammaCorrection(radiance);
    outColor = vec4(radiance, 1.0f);
}
// End MainSceneCookTorrance.frag