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
    const vec3 view,
    const vec3 normal,
    const vec3 lightDir,
    const vec3 lightColor)
{
    vec3 L = -lightDir;
    
    float NoL = clamp(dot(normal, L), 0.0f, 1.0f);
    
    vec3 brdf = calCookTorranceBRDF(view, normal, L, NoL);
    
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
    
    vec3 dir        = m_directionalLight.dir;
    vec3 lightColor = m_directionalLight.lightBase.color;

    
    // TODO: Calculate out color
    outColor = vec4(1.0f, 1.0f, 1,0f, albedo.w);
}
