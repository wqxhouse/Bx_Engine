/*
    MainSceneDefferedDraw.frag
*/
#version 440 core

#include <Light.hglsl>

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D texCoordTex;
uniform sampler2D kaTex;
uniform sampler2D kdTex;

uniform sampler2DMS shadowMapSampler;

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
    vec4 gPos      = texture(posTex, gBufferTexCoord);
    vec4 gNormal   = texture(normalTex, gBufferTexCoord);
    vec4 gTexCoord = texture(texCoordTex, gBufferTexCoord);
    
    // Get mesh data
    vec3 posWorld    = gPos.xyz;
    vec3 normalWorld = gNormal.xyz;
    vec2 texCoord    = gTexCoord.xy;
    
    // Get material data
    vec3 ka  = texture(kaTex, gBufferTexCoord).xyz;
    vec3 kd  = texture(kdTex, gBufferTexCoord).xyz;
    vec3 ks  = vec3(gPos.w, gNormal.w, gTexCoord.z);
    float ns = gTexCoord.w;
    
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
    vec3 specularCoefficient  = 
    //vec3(0.0f, 0.0f, 0.0f);
    ks * pow(VoR, ns);
        
    vec3 phongShdingColor = clamp(((ka + diffuseCoefficient + specularCoefficient) * lightColor), 0.0f, 1.0f);
    /// Finish Shading
    
    outColor = vec4(phongShdingColor, 1.0f);
}
