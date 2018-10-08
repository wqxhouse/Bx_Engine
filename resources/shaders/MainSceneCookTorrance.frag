/*
    MainSceneCookTorrance.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Material.hglsl>
#include <Utilities.hglsl>
#include <BRDF.hglsl>

in vec4 posWorldVec4;
in vec3 normalWorld;
in vec2 fragTexCoord;

// in vec4 posLightProj;

in vec3 lightProbeSampler;

uniform sampler2D diffuseMap;
uniform sampler2D specMap;
uniform sampler2D normalMap;
uniform sampler2D lightMap;

uniform sampler2DArray shadowMapSampler;
uniform samplerCube lightProbeCubemap;

//uniform sampler2D shadowMapSampler;
//uniform sampler2DMS shadowMapSampler;

/*uniform shadowMapResolutionUniformBlock
{
    Resolution m_shadowMapResolution;
};*/

layout (std140) uniform lightArrayUniformBlock
{
    Light m_light[MAX_LIGHT_NUM];
};

layout (std140) uniform material
{
	PhongMaterial m_phongMaterial;
};

uniform vec3 eyePos;

uniform int lightNum;

out vec4 outColor;

float castingShadow(vec4 posLightProj, float i)
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;

	float depth = texture(shadowMapSampler, vec3(posLight.xy, i)).r;
    
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
    const vec3  lightColor)
{
    vec3 L = -lightDir;
    
    float NoL = clamp(dot(normal, L), 0.0f, 1.0f);
    
    vec3 brdf = calCookTorranceBRDF(view, normal, L, NoL, m_cookTorranceMaterial, 1.0f);
    
    vec3 radiance = brdf * lightColor * NoL;
    
    return radiance;
}

void main()
{
	vec3 radiance;
    
	vec3 posWorld   = posWorldVec4.xyz / posWorldVec4.w;
    vec3 view       = normalize(eyePos - posWorld);
    vec3 normal     = normalize(normalWorld);

    vec3 reflection = normalize(2 * dot(normalWorld, view) * normalWorld - view);
    
    float attenuation = 1.0f;

	for (int i = 0; i < lightNum; ++i)
	{
        vec4  posLightProj      = m_light[i].lightBase.lightTransVP * posWorldVec4;
        float shadowAttenuation = castingShadow(posLightProj, float(i));

        vec3 lightColor = m_light[i].lightBase.color;

		vec3 dir;
        switch(m_light[i].lightBase.type)
        {
            case 0: // Directional Light
            {
                // Transform light direction vector to view space
                dir = normalize(m_light[i].data[0].xyz);
                shadowAttenuation = 1.0f;                
                break;
            }
            case 1: // Point Light
            {
                float radius  = m_light[i].data[0].w;
                float radius2 = radius * radius;

                // Transform light position vector to view space
                vec3  dirVector = posWorld - m_light[i].data[0].xyz;
                float dis2      = dot(dirVector, dirVector);
                shadowAttenuation = 1.0f;

                // Pixel is outside the range of point light, discard
                if (dis2 > radius2) { continue; }

                dir = normalize(dirVector);

                break;
            }
            case 2: // Spot Light
            {
                vec3  lightDir      = normalize(m_light[i].data[1].xyz);
                float innerCosTheta = m_light[i].data[0].w;
                float outerCosTheta = m_light[i].data[1].w;

                vec3  dirVector = posWorld - m_light[i].data[0].xyz;                
                dir             = normalize(dirVector);

                float cosTheta = dot(lightDir, dir);

                // Pixel is outside the range of spot light, discard
                if      (cosTheta < outerCosTheta) { continue; }
                // Pixel is between the inner and outer theta range, add attenuation effect
                else if (cosTheta < innerCosTheta) { attenuation = 0.8f; }

                // TODO: Distance check
                // float dis2      = dot(dirVector, dirVector);
                // float cosTheta2 = cosTheta * cosTheta;
                // dis2           *= cosTheta2;
                // 
                // float range  = m_light[i].lightBase.padding1.y;
                // float range2 = range * range;

                // Pixel is outside the range of spot light, discard
                // if (dis2 > range2) { continue; }

                break;
            }
            default:
                break;
        }
        //float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

        vec3 lightRadiance = calCookTorranceRadiance(view, normal, dir, lightColor);
        
        // Shadow casting
        // lightRadiance *= shadowAttenuation;
        
        radiance += ((shadowAttenuation < 0.9999999f) ? vec3(0.0f) : lightRadiance * attenuation);
        // radiance += lightRadiance;
    }

    // Light Probe
    // 0 - 1 roughtness; 0 - 7 mipmap
    vec3 environmentLight = texture(lightProbeCubemap, reflection, m_cookTorranceMaterial.roughness * 7.0f).xyz;
        
    // Fetch environmentLight
    vec3 environmentLightRadiance = calCookTorranceRadiance(view, normal, -reflection, environmentLight);
    //radiance += environmentLightRadiance;
    
    //radiance *= texture(diffuseMap, fragTexCoord).xyz;
    
    // Gamma correction
    radiance = gammaCorrection(radiance);
    outColor = vec4(radiance, 1.0f);
}
// End MainSceneCookTorrance.frag