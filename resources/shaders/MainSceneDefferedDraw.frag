/*
    MainSceneDefferedDraw.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Utilities.hglsl>
#include <BRDF.hglsl>
#include <SSAO.hglsl>

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D texCoordTex;
uniform sampler2D albedoTex;
uniform sampler2D specularTex;
uniform sampler2D environmentLightTex;

uniform sampler2D noiseTex;

layout (std140) uniform directionalLightUniformBlock
{
    DirectionalLight m_directionalLight;
};

layout (std140) uniform pointLightUniformBlock
{
    PointLight m_pointLight;
};

uniform vec3 eyePos;
uniform mat4 projMat;

uniform uint testSampleNum;
uniform vec3 testSamples[64];

//uniform uint screenWidth;
//uniform uint screenHeight;

out vec4 outColor;

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

vec2 calgBufferTexCoord()
{
    vec2 result;
    
    result = gl_FragCoord.xy;
    
    // TODO: Send screen size from setting
    result.x /= 1280.0f;
    result.y /= 720.0f;
    
    return result;
}

float calSsao(
    const vec3  pos,        // position
    const vec3  normal,     // Normal
    const vec2  texCoord)   // Texture coordinate
{
    float occlusion = testSampleNum;//m_ssaoSample.sampleNum;
    
    vec2 ssaoTexCoord = vec2(texCoord.x * 1280.0f, texCoord.y * 720.0f);
    
    vec3 randomVec = normalize(texture(noiseTex, ssaoTexCoord)).xyz;
    vec3 tangent   = normalize(randomVec - normal * dot(normal, randomVec)); // X axis
    vec3 biTangent = cross(normal, tangent);                                  // Y axis
    
    mat3 TBN = mat3(tangent, biTangent, normal);
    
    for (uint i = 0; i < testSampleNum/*m_ssaoSample.sampleNum*/; ++i)
    {
        vec3 sampleVec = TBN * testSamples[i];//m_ssaoSample.samples[i];
        
        vec3 samplePos = pos + sampleVec;
        
        vec4 samplePosVec4     = vec4(samplePos, 1.0f);
        vec4 samplePosProjVec4 = projMat * samplePosVec4;
        samplePosProjVec4.xy  /= samplePosProjVec4.w;
        samplePosProjVec4.xy   = 0.5f * samplePosProjVec4.xy + 0.5f;
        
        float depth = texture(posTex, samplePosProjVec4.xy).z;
        
        if (samplePos.z > depth) // occlusion
        {
            float scale = clamp(length(sampleVec), 0.0f, 1.0f);
            occlusion -= scale;
        }
    }
    
    occlusion /= float(/*m_ssaoSample.sampleNum*/testSampleNum);
    
    return occlusion;
}

void main()
{
    vec2 gBufferTexCoord = calgBufferTexCoord();
    
    // Sample from G-Buffer
    // Get mesh data
    vec3 posWorld    = texture(posTex, gBufferTexCoord).xyz;
    vec3 normalWorld = texture(normalTex, gBufferTexCoord).xyz;

    vec3 gTexCoord   = texture(texCoordTex, gBufferTexCoord).xyz;
    vec2 texCoord    = gTexCoord.xy;

    // Get material data
    vec3 environmentLight  = texture(environmentLightTex, gBufferTexCoord).xyz;

    vec4 gAlbedo = texture(albedoTex, gBufferTexCoord);
    vec3 albedo  = gAlbedo.xyz;

    vec4  gSpecular = texture(specularTex, gBufferTexCoord);
    vec3  specular  = gSpecular.xyz;
    float ns  = gSpecular.w;
    
    /// Shading
    vec3 view       = normalize(eyePos - posWorld);
    vec3 normal     = normalize(normalWorld);    
    vec3 dir        = m_directionalLight.dir;
    vec3 lightColor = m_directionalLight.lightBase.color;
        
    // Casting shadow
    float shadowAttenuation   = gTexCoord.z;
    float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);
    
    vec3 radiance;
    
    if (ns > 0.0f)
    {
        // Calculate diffuse color
        float NoL                = clamp(dot(normal, -dir), 0.0f, 1.0f);
        vec3 diffuseCoefficient  = albedo * NoL;
        
        // Calculate specular color
        vec3 reflection           = normalize(2 * NoL * normal + dir);    
        float VoR                 = clamp(dot(view, reflection), 0.0f, 1.0f);    
        vec3 specularCoefficient  = specular * pow(VoR, ns);

        specularCoefficient  *= shadowSpecularAttenuation;
        vec3 phongShdingColor = clamp(((environmentLight + diffuseCoefficient + specularCoefficient) * lightColor), 0.0f, 1.0f);
        /// Finish Shading
        
        radiance = phongShdingColor;
        
        phongShdingColor *= shadowAttenuation;
    }
    else
    {
        // Get material data
        float roughness = specular.x;
        float matellic  = specular.y;
        float fresnel   = specular.z;
        
        CookTorranceMaterial material = { albedo.xyz, 1.0f, roughness, matellic, fresnel };
        
        radiance = calCookTorranceRadiance(view, normal, dir, lightColor, material, shadowSpecularAttenuation);
    }
        
    // Shadow casting
    radiance *= shadowAttenuation;
    
    // TODO: Real-time open and close SSAO
    // SSAO
    float occlusion = calSsao(posWorld, normalWorld, texCoord);
    radiance *= occlusion;
    
    // Gamma correction
    radiance = gammaCorrection(radiance);
    
    outColor = vec4(radiance, gAlbedo.w);
}
