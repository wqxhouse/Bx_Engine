/*
    MainSceneDefferedDraw.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Utilities.hglsl>
#include <BRDF.hglsl>
#include <Frustum.hglsl>

#define RENDER_METHOD_DEFERRED       0u
#define RENDER_METHOD_DEFERRED_TILED 1u

layout(location = 0) uniform sampler2D posTex;
layout(location = 1) uniform sampler2D normalTex;
layout(location = 2) uniform sampler2D texCoordTex;
layout(location = 3) uniform sampler2D albedoTex;
layout(location = 4) uniform sampler2D specularTex;
layout(location = 5) uniform sampler2D environmentLightTex;

// Lightprobe cubemap
layout(location = 6) uniform samplerCube lightProbeCubemap;

// SSAO Texture
layout(location = 7) uniform sampler2D ssaoTex;

// Shadow map
layout (location = 8) uniform sampler2DArray shadowMapSampler;
//layout (location = 8) uniform sampler2DMS shadowMapSampler;

/*layout (std140) uniform ShadowMapResolutionUniformBlock
{
    Resolution m_shadowMapResolution;
};*/

layout (std140) uniform RenderingResolutionBlock
{
    Resolution m_resolution;
};

uniform vec3 eyePos;

uniform mat4 viewMat;

// uniform mat4 lightTransVP;

// SSAO
uniform int useSsao;

// Tiled rendering
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

out vec4 outColor;

float castingShadow(vec4 posLightProj, float i)
{
	float shadowAttenuation = 1.0f;

	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;

	// TODO: Added control for enable/disable shadow anti-alasing.

    //float depth = 1.0f;
	float depth = texture(shadowMapSampler, vec3(posLight.xy, i)).r;

    // Multisampling, need integer coordinate
    /*
    float depth = 0.0f;

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
    const vec3                 view,
    const vec3                 normal,
    const vec3                 lightDir,
    const vec3                 lightColor,
    const CookTorranceMaterial material)
{
    vec3 L = -lightDir;

    float NoL = clamp(dot(normal, L), 0.0f, 1.0f);

    vec3 brdf = calCookTorranceBRDF(view, normal, L, NoL, material, 1.0f);

    vec3 radiance = brdf * lightColor * NoL;

    return radiance;
}

vec2 calgBufferTexCoord()
{
    vec2 result;

    result = gl_FragCoord.xy;
    result.x /= float(m_resolution.width);
    result.y /= float(m_resolution.height);

    return result;
}

void main()
{
    vec2 gBufferTexCoord = calgBufferTexCoord();

    // Sample from G-Buffer
    // Get mesh data
    vec4 posWorldVec4 = texture(posTex, gBufferTexCoord);

    vec4 posViewVec4 = viewMat * posWorldVec4;
    vec3 posView    = posViewVec4.xyz / posViewVec4.w;
    vec3 normalView = texture(normalTex, gBufferTexCoord).xyz;

    vec3 gTexCoord   = texture(texCoordTex, gBufferTexCoord).xyz;
    vec2 texCoord    = gTexCoord.xy;

    vec4 gAlbedo = texture(albedoTex, gBufferTexCoord);
    vec3 albedo  = gAlbedo.xyz;

    vec4  gSpecular = texture(specularTex, gBufferTexCoord);
    vec3  specular  = gSpecular.xyz;
    float ns        = gSpecular.w;

    // Transform eye position to view space
    vec4 eyePosVec4 = viewMat * vec4(eyePos, 1.0f);
    vec3 eyePosView = eyePosVec4.xyz / eyePosVec4.w;

    /// Shading
    vec3 radiance; // Final radiance for every pixel from hemisphere
    float attenuation = 1.0f; // Attenuations for radiance
	
    uint validLightNum = m_lightBuffer.lightNum;
    /// Tiled branch
    ///
    //  We flip the x here for transforming LH coordinate system back to RH system.
    ///
    LightTile lightGrid;
    if (useForwardPlus == RENDER_METHOD_DEFERRED_TILED)
    {
        uvec2 tileIndexD2 = uvec2((m_resolution.width - uint(gl_FragCoord.x)) / m_tileSize.width,
                                  uint(gl_FragCoord.y) / m_tileSize.height);

        uint tileIndex = tileIndexD2.x + tileIndexD2.y * m_tileResolution.width;

        lightGrid     = m_lightGrid[tileIndex];
        validLightNum = lightGrid.size;
    }
    /// End Tiled branch

	// Loop all lights
	for (uint i = 0; i < validLightNum; ++i)
	{
        Light light;

        /// Tiled branch
        if (useForwardPlus == RENDER_METHOD_DEFERRED_TILED)
        {
            uint index      = lightGrid.offset + i;
            uint lightIndex = m_lightIndexList[index];
            light           = m_lightBuffer.m_light[lightIndex];
        }
        /// End tiled
        else if (useForwardPlus == RENDER_METHOD_DEFERRED)
        {
            light = m_lightBuffer.m_light[i];
        }

		// Shadow casting
		vec4  posLightProj      = light.lightBase.lightTransVP * posWorldVec4;
		float shadowAttenuation = castingShadow(posLightProj, float(i));
		
		vec3 lightColor = light.lightBase.color;

        vec3 dir;
        switch(light.lightBase.type)
        {
            case 0: // Directional Light
            {
                // Transform light direction vector to view space
                dir = normalize(viewMat * vec4(light.data[0].xyz, 0.0f)).xyz;
                
                // TODO: Enable shadow for directional lights
                shadowAttenuation = 1.0f;
                
                break;
            }
            case 1: // Point Light
            {
                float radius  = light.data[0].w;
                float radius2 = radius * radius;

                // Transform light position vector to view space
                vec4 lightViewPosVec4 = viewMat * vec4(light.data[0].xyz, 1.0f);
                vec3 lightViewPos     = lightViewPosVec4.xyz / lightViewPosVec4.w;

                vec3  dirVector = posView - lightViewPos;
                float dis2      = dot(dirVector, dirVector);

                // Pixel is outside the range of point light, discard
                if (dis2 > radius2) { continue; }

                dir = normalize(dirVector);
                
                // TODO: Enable shadow for directional lights
                shadowAttenuation = 1.0f;
                
                break;
            }
            case 2: // Spot Light
            {
                vec3  lightDir      = normalize(viewMat * vec4(light.data[1].xyz, 0.0f)).xyz;
                float innerCosTheta = light.data[0].w;
                float outerCosTheta = light.data[1].w;
                
                // Transform light position vector to view space
                vec4 lightViewPosVec4 = viewMat * vec4(light.data[0].xyz, 1.0f);
                vec3 lightViewPos     = lightViewPosVec4.xyz / lightViewPosVec4.w;
                
                vec3 dirVector = posView - lightViewPos;
                dir            = normalize(dirVector);
                
                float cosTheta = dot(lightDir, dir);

                // Pixel is outside the range of spot light, discard
                if      (cosTheta < outerCosTheta) { continue; }
                // Pixel is between the inner and outer theta range, add attenuation effect
                else if (cosTheta < innerCosTheta) { attenuation = 0.8f; }

                break;
            }
            default:
                break;
        }

		// Casting shadow
		// float shadowAttenuation = gTexCoord.z;
		float shadowSpecularAttenuation = 1.0f;//((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

		vec3 view       = normalize(eyePosView - posView);
		vec3 normal     = normalize(normalView);

		if (ns > 0.0f)
		{
			// Get material data
			vec3 environmentLight  = texture(environmentLightTex, gBufferTexCoord).xyz;

			// Calculate diffuse color
			float NoL                = clamp(dot(normal, -dir), 0.0f, 1.0f);
			vec3 diffuseCoefficient  = albedo * NoL;

			// Calculate specular color
			vec3 reflection           = normalize(2 * NoL * normal + dir);    
			float VoR                 = clamp(dot(view, reflection), 0.0f, 1.0f);    
			vec3 specularCoefficient  = specular * pow(VoR, ns);

			specularCoefficient  *= shadowSpecularAttenuation;
			vec3 phongShdingColor =
				clamp(((environmentLight + diffuseCoefficient + specularCoefficient) * lightColor), 0.0f, 1.0f);

			phongShdingColor *= shadowAttenuation;
			radiance += phongShdingColor * attenuation;
		}
		else
		{
			// Get material data
			float roughness = specular.x;
			float matellic  = specular.y;
			float fresnel   = specular.z;

			CookTorranceMaterial material = { albedo.xyz, 1, roughness, matellic, fresnel, 1 };

			vec3 directLightRadiance = calCookTorranceRadiance(view, normal, dir, lightColor, material);

			// Shadow casting
			radiance += ((shadowAttenuation < 0.9999999f) ? vec3(0.0f) : (directLightRadiance * attenuation));

            if (i == 0)
            {
                //vec3 reflection               = normalize(2 * dot(normal, view) * normal - view);
                vec3 reflection               = texture(environmentLightTex, gBufferTexCoord).xyz;
                vec3 environmentLight         = texture(lightProbeCubemap, reflection, material.roughness * 7.0f).xyz;
                
                vec3 reflectionView           = (viewMat * vec4(reflection, 0.0f)).xyz;
                vec3 environmentLightRadiance =
                    calCookTorranceRadiance(view, normal, -reflectionView, environmentLight, material);

                // radiance += environmentLightRadiance;
            }
		}
	}
    /// Finish Shading

    // SSAO
    if (useSsao == 1)
    {
        float occlusion = texture(ssaoTex, gBufferTexCoord).r;

        radiance *= occlusion;
    }
    
    // Gamma correction
    radiance = gammaCorrection(radiance);
    
    outColor = vec4(radiance, gAlbedo.w);
}
// End of MainSceneDefferedDraw.frag