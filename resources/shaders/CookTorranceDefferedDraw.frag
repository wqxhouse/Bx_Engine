/*
    MainSceneDefferedDraw.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Utilities.hglsl>
#include <BRDF.hglsl>

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D texCoordTex;
uniform sampler2D specularTex;
uniform sampler2D albedoTex;
uniform sampler2D specularTex;
uniform sampler2D environmentLightTex;

layout (std140) uniform directionalLightUniformBlock
{
    DirectionalLight m_directionalLight;
};

layout (std140) uniform pointLightUniformBlock
{
    PointLight m_pointLight;
};

uniform vec3 eyePos;
//uniform uint screenWidth;
//uniform uint screenHeight;

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

vec2 calgBufferTexCoord()
{
    vec2 result;
    
    result = gl_FragCoord.xy;
    
    // TODO: Send screen size from setting
    result.x /= 1280.0f;
    result.y /= 720.0f;
    
    return result;
}

vec3 calCookTorranceRadiance(
    const vec3                 view,
    const vec3                 normal,
    const vec3                 lightDir,
    const vec3                 lightColor,
    const CookTorranceMaterial material,
    const float                shadowSpecularAttenuation)
{
    vec3 L = -lightDir;
    
    float NoL = clamp(dot(normal, L), 0.0f, 1.0f);
    
    vec3 brdf = calCookTorranceBRDF(view, normal, L, NoL, material, 1.0f);
    
    vec3 radiance = brdf * lightColor * NoL;
    
    return radiance;
}

void main()
{
    vec2 gBufferTexCoord = calgBufferTexCoord();
    
    // Sample from G-Buffer
    vec3 posWorld         = texture(posTex, gBufferTexCoord);
    vec3 normalWorld      = texture(normalTex, gBufferTexCoord);
    vec2 texCoord         = texture(texCoordTex, gBufferTexCoord);
    vec4 albedo           = texture(albedoTexture, gBufferTexCoord);
    vec3 specular         = texture(specularTexture, gBufferTexCoord).xyz;
    vec3 environmentLight = texture(environmentLightTex, gBufferTexCoord);
    
    // Get material data
    float roughness = specular.x;
    float matellic  = specular.y;
    float fresnel   = specular.z;
    
    /// Shading
    vec3 view       = normalize(eyePos - posWorld);
    vec3 normal     = normalize(normalWorld);
    vec3 dir        = m_directionalLight.dir;
    vec3 lightColor = m_directionalLight.lightBase.color;

    CookTorranceMaterial material = { albedo.xyz, 1.0f, roughness, matellic, fresnel };
    
    float shadowAttenuation         = castingShadow();
    float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);
    
    vec3 radiance = calCookTorranceRadiance(view, normal, dir, lightColor, material, shadowSpecularAttenuation);
    
    // Shadow casting
    radiance *= shadowAttenuation;
    
    // Gamma correction
    radiance = gammaCorrection(radiance);
    
    outColor = vec4(radiance, 1.0f);
}
