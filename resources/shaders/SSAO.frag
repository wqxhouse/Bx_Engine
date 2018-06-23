/*
    SSAO.frag
*/
#version 440 core

#define SSAO_SAMPLE_MAX 256

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D texCoordTex;

uniform sampler2D noiseTex;

/*struct SsaoSample
{
    uint sampleNum; vec3 padding;
    vec3 samples[SSAO_SAMPLE_MAX];
};

layout (std140) uniform SsaoSamplesUniformBlock
{
    SsaoSample m_ssaoSample;
};*/

uniform mat4 viewMat;
uniform mat4 projMat;

uniform uint testSampleNum;
uniform vec3 testSamples[64];

layout(location = 0) out float ssaoTexture;

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
    
    vec3 posWorld    = texture(posTex, gBufferTexCoord).xyz;
    vec3 normalWorld = texture(normalTex, gBufferTexCoord).xyz;
    
    vec3 posView    = (viewMat * vec4(posWorld, 1.0f)).xyz;
    vec3 normalView = (viewMat * vec4(normalWorld, 1.0f)).xyz;
    
    float occlusion = testSampleNum;//m_ssaoSample.sampleNum;
    
    vec2 ssaoTexCoord = gl_FragCoord.xy;
    
    vec3 randomVec = normalize(texture(noiseTex, ssaoTexCoord)).xyz;
    vec3 tangent   = normalize(randomVec - normalView * dot(normalView, randomVec)); // X axis
    vec3 biTangent = cross(normalView, tangent);                                     // Y axis
    
    mat3 TBN = mat3(tangent, biTangent, normalView);
    
    for (uint i = 0; i < testSampleNum/*m_ssaoSample.sampleNum*/; ++i)
    {
        vec3 sampleVec = TBN * testSamples[i];//m_ssaoSample.samples[i];
        
        vec3 samplePos = posView + sampleVec;
        
        vec4 samplePosVec4     = vec4(samplePos, 1.0f);
        vec4 samplePosProjVec4 = projMat * samplePosVec4;
        samplePosProjVec4.xy  /= samplePosProjVec4.w;
        samplePosProjVec4.xy   = 0.5f * samplePosProjVec4.xy + 0.5f;
        
        vec4 depthPosWorld = texture(posTex, samplePosProjVec4.xy);
        float depth = (viewMat * depthPosWorld).z;
        
        if (samplePos.z > depth) // occlusion
        {
            occlusion -= 1.0f;
        }
    }
    
    occlusion /= 64.0f;//float(/*m_ssaoSample.sampleNum*/testSampleNum);
    
    ssaoTexture = occlusion;
}