/*
    SSAO.frag
*/
#version 440 core

#define SSAO_SAMPLE_MAX 256

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D texCoordTex;

uniform sampler2D noiseTex;

struct SsaoSample
{
    uint sampleNum;
    vec3 samples[SSAO_SAMPLE_MAX];
};

uniform SsaoSamplesUniformBlock
{
    SsaoSample m_ssaoSample;
};

uniform mat4 projMat;

layout(location = 0) out vec3 ssaoTexture;

vec2 calgBufferTexCoord()
{
    vec2 result;
    
    result = gl_FragCoord.xy;
    
    // TODO: Send screen size from setting
    result.x /= 1280.0f;
    result.y /= 720.0f;
    
    return result;
}

// Test
vec2 tileDeminsion = vec2(1280.0f / 4.0f, 720.0f / 4.0f);

void main()
{
    vec2 gBufferTexCoord = calgBufferTexCoord();
    
    vec2 texCoord    = texture(texCoordTex, gBufferTexCoord).xy;
    
    vec3 posView    = texture(posTex, gBufferTexCoord).xyz;
    vec3 normalView = texture(normalTex, gBufferTexCoord).xyz;
    
    float occlusion = m_ssaoSample.sampleNum;
    
    vec2 ssaoTexCoord = texCoord * tileDeminsion;    
    vec3 randomVec = normalize(texture(noiseTex, ssaoTexCoord)).xyz;
    vec3 tangent   = normalize(randomVec - normalView * dot(normalView, randomVec)); // X axis
    vec3 biTangent = cross(normalView, tangent);                                     // Y axis
    
    mat3 TBN = mat3(tangent, biTangent, normalView);
    
    for (uint i = 0; i < m_ssaoSample.sampleNum; ++i)
    {
        vec3 sampleVec = TBN * m_ssaoSample.samples[i];
        vec3 samplePos = posView + sampleVec;
        
        vec4 samplePosVec4     = vec4(samplePos, 1.0f);
        vec4 samplePosProjVec4 = projMat * samplePosVec4;
        samplePosProjVec4.xy  /= samplePosProjVec4.w;
        samplePosProjVec4.xy   = 0.5f * samplePosProjVec4.xy + 0.5f;
        
        vec4 depthGPosWorld = texture(posTex, samplePosProjVec4.xy);
        
        vec3 depthPosWorld;
        vec3 depthPosView = depthGPosWorld.xyz;
        
        if (depthGPosWorld.w > 0.99999f)
        {            
            float depth = depthPosView.z;

            if (samplePos.z < depth + 0.001f) // occlusion
            {
                occlusion -= 1.0f;
            }
        }
    }
    
    occlusion /= 64.0f;
    
    ssaoTexture = vec3(occlusion);
}