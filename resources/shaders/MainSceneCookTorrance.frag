/*
    MainSceneCookTorrance.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Material.hglsl>
#include <Utilities.hglsl>
#include <BRDF.hglsl>
#include <Frustum.hglsl>

#define RENDER_METHOD_FORWARD      0u
#define RENDER_METHOD_FORWARD_PLUS 1u

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

layout (std140) uniform RenderingResolutionBlock
{
    Resolution m_resolution;
};

layout (std140) uniform material
{
	PhongMaterial m_phongMaterial;
};

uniform vec3 eyePos;

out vec4 outColor;

// Forward+
uniform uint useForwardPlus;
layout(std140) uniform TileSizeUniformBlock
{
    Resolution m_tileSize; // 16 * 16 by default
};

layout(std140) uniform GlobalSizeUniformBlock
{
    Resolution m_tileResolution;
};

layout(std430, binding = 1) buffer LightGridBuffer
{
    LightTile m_lightGrid[];
};

layout(std430, binding = 2) buffer LightIndexListBuffer
{
    uint m_lightIndexList[];
};

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

    CookTorranceMaterial cookTorranceMaterial = m_cookTorranceMaterial;
    cookTorranceMaterial.albedo = texture(diffuseMap, fragTexCoord).xyz;

    vec3 brdf = calCookTorranceBRDF(view, normal, L, NoL, cookTorranceMaterial, 1.0f);

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

    uint validLightNum = m_lightBuffer.lightNum;
    /// Forward+ branch
    ///
    //  We flip the x here for transforming LH coordinate system back to RH system.
    ///
    LightTile lightGrid;

    if (useForwardPlus == RENDER_METHOD_FORWARD_PLUS)
    {
        uvec2 tileIndexD2 = uvec2((m_resolution.width - uint(gl_FragCoord.x)) / m_tileSize.width,
                                  uint(gl_FragCoord.y) / m_tileSize.height);

        uint tileIndex = tileIndexD2.x + tileIndexD2.y * m_tileResolution.width;

        lightGrid     = m_lightGrid[tileIndex];
        validLightNum = lightGrid.size;
    }
    /// Forward+ branch End

	for (uint i = 0; i < validLightNum; ++i)
	{
        Light light;
        
        /// Forward+ branch
        if (useForwardPlus == RENDER_METHOD_FORWARD_PLUS)
        {
            uint index      = lightGrid.offset + i;
            uint lightIndex = m_lightIndexList[index];
            light           = m_lightBuffer.m_light[lightIndex];
        }
        else if(useForwardPlus == RENDER_METHOD_FORWARD) // Normal Forward rendering
        {
            light = m_lightBuffer.m_light[i];
        }
        
        vec4  posLightProj      = light.lightBase.lightTransVP * posWorldVec4;
        float shadowAttenuation = castingShadow(posLightProj, float(i));

        vec3 lightColor = light.lightBase.color;

		vec3 dir;
        switch(light.lightBase.type)
        {
            case 0: // Directional Light
            {
                // Transform light direction vector to view space
                dir = normalize(light.data[0].xyz);

                // TODO: Enable shadow for directional lights
                shadowAttenuation = 1.0f;
                break;
            }
            case 1: // Point Light
            {
                float radius  = light.data[0].w;
                float radius2 = radius * radius;

                // Transform light position vector to view space
                vec3  dirVector = posWorld - light.data[0].xyz;
                float dis2      = dot(dirVector, dirVector);

                // TODO: Enable shadow for directional lights
                shadowAttenuation = 1.0f;

                // Pixel is outside the range of point light, discard
                if (dis2 > radius2) { continue; }

                dir = normalize(dirVector);

                break;
            }
            case 2: // Spot Light
            {
                vec3  lightDir      = normalize(light.data[1].xyz);
                float innerCosTheta = light.data[0].w;
                float outerCosTheta = light.data[1].w;

                vec3  dirVector = posWorld - light.data[0].xyz;                
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
                // float range  = light.lightBase.padding1.y;
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

    /*if (validLightNum == 1) outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (validLightNum == 2) outColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (validLightNum == 3) outColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    else if (validLightNum == 4) outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);*/
}
// End MainSceneCookTorrance.frag