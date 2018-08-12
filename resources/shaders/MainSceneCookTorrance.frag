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

layout (std140) uniform directionalLightUniformBlock
{
    DirectionalLight m_directionalLight;
};

layout (std140) uniform pointLightUniformBlock
{
    PointLight m_pointLight;
};

layout (std140) uniform material
{
	PhongMaterial m_phongMaterial;
};

uniform shadowMapResolutionUniformBlock
{
    ShadowMapResolution m_shadowMapResolution;
};

uniform vec3 eyePos;

out vec4 outColor;

float castingShadow()
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;
	
    // Multisampling, need integer coordinate
    //posLight.x = posLight.x * 2560.0f; // m_shadowMapResolution.width;
    //posLight.y = posLight.y * 2560.0f; // m_shadowMapResolution.height;

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
    //vec3  dis     = posWorld - m_pointLight.pos;
    //float dis2    = dot(dis, dis);
    //float radius2 = m_pointLight.radius * m_pointLight.radius;
    
    //vec3 dir      = normalize(dis);
    
    //if (dis2 <= radius2)
    {
        //vec3 lightColor = m_pointLight.lightBase.color;
        
        vec3 view       = normalize(eyePos - posWorld);
        vec3 normal     = normalize(normalWorld);
        vec3 dir        = normalize(m_directionalLight.dir);
        vec3 lightColor = m_directionalLight.lightBase.color;
        
        vec3 reflection = normalize(2 * dot(normalWorld, view) * normalWorld - view);
        
        // Light Probe
        vec3 environmentLight = texture(lightProbeCubemap, reflection).xyz;
        lightColor += environmentLight;
        
        float shadowAttenuation         = castingShadow();
        float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

        vec3 radiance = calCookTorranceRadiance(view, normal, dir, lightColor, shadowSpecularAttenuation);
        
        // Shadow casting
        radiance *= shadowAttenuation;
        
        // Gamma correction
        radiance = gammaCorrection(radiance);
        
        outColor = vec4(radiance, 1.0f);
        
        // outColor = vec4(environmentLight, 1.0f);
    }
    //else
    {
        //outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}
