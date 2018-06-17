/*
    MainSceneDefferedDraw.frag
*/
#version 440 core

#include <Light.hglsl>
#include <Utilities.hglsl>

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D texCoordTex;
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
    vec3 ka  = texture(environmentLightTex, gBufferTexCoord).xyz;

    vec4 gKd = texture(albedoTex, gBufferTexCoord);
    vec3 kd  = gKd.xyz;

    vec4  gKs = texture(specularTex, gBufferTexCoord);
    vec3  ks  = gKs.xyz;
    float ns  = gKs.w;
    
    /// Shading
    vec3 view       = normalize(eyePos - posWorld);
    
    vec3 dir        = m_directionalLight.dir;
    vec3 lightColor = m_directionalLight.lightBase.color;

    // Calculate diffuse color
    float NoL                = clamp(dot(normalWorld, -dir), 0.0f, 1.0f);
    vec3 diffuseCoefficient  = kd * NoL;
    
    // Calculate specular color
    vec3 reflection           = normalize(2 * NoL * normalWorld + dir);    
    float VoR                 = clamp(dot(view, reflection), 0.0f, 1.0f);    
    vec3 specularCoefficient  = ks * pow(VoR, ns);
    
    // Casting shadow
    float shadowAttenuation   = gTexCoord.z;
    float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

    specularCoefficient  *= shadowSpecularAttenuation;
    vec3 phongShdingColor = clamp(((ka + diffuseCoefficient + specularCoefficient) * lightColor), 0.0f, 1.0f);

    phongShdingColor *= shadowAttenuation;
    /// Finish Shading

    // Gamma correction
    phongShdingColor = gammaCorrection(phongShdingColor);
    
    outColor = vec4(phongShdingColor, gKd.w);
}
