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
uniform sampler2D albedoTex;
uniform sampler2D specularTex;
uniform sampler2D environmentLightTex;

// SSAO Texture
uniform sampler2D ssaoTex;

// Lightprobe cubemap
uniform samplerCube lightProbeCubemap;

layout (std140) uniform directionalLightUniformBlock
{
    DirectionalLight m_directionalLight[MAX_LIGHT_NUM];
};

layout (std140) uniform pointLightUniformBlock
{
    PointLight m_pointLight;
};

layout (std140) uniform RenderingResolutionBlock
{
    uniform int resolutionWidth;
    uniform int resolutionHeight;
};

uniform vec3 eyePos;

uniform mat4 viewMat;

out vec4 outColor;

// Test
uniform int useSsao;

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
    result.x /= float(resolutionWidth); //1280.0f;
    result.y /= float(resolutionHeight); //720.0f;
    
    return result;
}

void main()
{
    vec2 gBufferTexCoord = calgBufferTexCoord();

    // Sample from G-Buffer
    // Get mesh data
    vec3 posView    = texture(posTex, gBufferTexCoord).xyz;
    vec3 normalView = texture(normalTex, gBufferTexCoord).xyz;

    vec3 gTexCoord   = texture(texCoordTex, gBufferTexCoord).xyz;
    vec2 texCoord    = gTexCoord.xy;

    vec4 gAlbedo = texture(albedoTex, gBufferTexCoord);
    vec3 albedo  = gAlbedo.xyz;

    vec4  gSpecular = texture(specularTex, gBufferTexCoord);
    vec3  specular  = gSpecular.xyz;
    float ns  = gSpecular.w;

    // Transform eye position to view space
    vec4 eyePosVec4 = viewMat * vec4(eyePos, 1.0f);
    vec3 eyePosView = eyePosVec4.xyz / eyePosVec4.w;

    // Transform light direction vector to view space
    vec3 dir   = normalize(viewMat * vec4(m_directionalLight[0].dir, 0.0f)).xyz;

    // Casting shadow
    float shadowAttenuation   = gTexCoord.z;
    float shadowSpecularAttenuation = 1.0f;// ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

    /// Shading
    vec3 radiance; // Final radiance for every pixel from hemisphere

    vec3 view       = normalize(eyePosView - posView);
    vec3 normal     = normalize(normalView);
    vec3 lightColor = m_directionalLight[0].lightBase.color;

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
        radiance = phongShdingColor;
    }
    else
    {
        // Get material data
        float roughness = specular.x;
        float matellic  = specular.y;
        float fresnel   = specular.z;

        CookTorranceMaterial material = { albedo.xyz, 1.0f, roughness, matellic, fresnel };

        vec3 directLightRadiance = calCookTorranceRadiance(view, normal, dir, lightColor, material, shadowSpecularAttenuation);
        // Shadow casting
        directLightRadiance *= shadowAttenuation;

        // vec3 reflection               = normalize(2 * dot(normal, view) * normal - view);
        vec3 reflection               = texture(environmentLightTex, gBufferTexCoord).xyz;
        vec3 environmentLight         = texture(lightProbeCubemap, reflection, material.roughness * 7.0f).xyz;
        
        vec3 reflectionView           = (viewMat * vec4(reflection, 0.0f)).xyz;
        vec3 environmentLightRadiance =
            calCookTorranceRadiance(view, normal, -reflectionView, environmentLight, material, shadowSpecularAttenuation);

        radiance = directLightRadiance + environmentLightRadiance;
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