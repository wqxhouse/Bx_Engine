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

//uniform mat4 viewMat;
uniform mat4 projMat;

uniform uint testSampleNum;
uniform vec3 testSamples[64];

// if (samplePos.z > depth) // occlusion
//{
      //occlusion = 0.0f;
//}

//depthTex = depthGPosWorld;
//depthViewTex.z = depth; // 2
//samplePosTex = samplePos;//vec3(samplePos.x + 5.0f, samplePos.y + 5.0f, samplePos.z); // 3
//
//posWorldTex = posWorld.xyz; // 4
//posViewTex = posView.xyz; // 5
//
//sampleVecTex = sampleVec; // 6
//samplePosProjTex.xy = samplePosProjVec4.xy; // 7

layout(location = 0) out float ssaoTexture;
///layout(location = 1) out vec4 depthTex;
///layout(location = 2) out vec3 depthViewTex;
///layout(location = 3) out vec3 samplePosTex;
//layout(location = 4) out vec3 posWorldTex;
//layout(location = 5) out vec3 posViewTex;
///layout(location = 4) out vec3 sampleVecTex;
//layout(location = 7) out vec3 samplePosProjTex;
///layout(location = 5) out vec3 xAxis;
///layout(location = 6) out vec3 yAxis;
///layout(location = 7) out vec3 zAxis;

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
    
    //vec3 posWorld    = texture(posTex, gBufferTexCoord).xyz;
    //vec3 normalWorld = texture(normalTex, gBufferTexCoord).xyz;
    vec2 texCoord    = texture(texCoordTex, gBufferTexCoord).xy;
    
    //vec4 posViewVec4    = viewMat * vec4(posWorld, 1.0f);
    //vec4 normalViewVec4 = viewMat * vec4(normalWorld, 1.0f);
    
    //vec3 posView    = posViewVec4.xyz / posViewVec4.w;
    //vec3 normalView = normalize(normalViewVec4.xyz / normalViewVec4.w);
    
    vec3 posView    = texture(posTex, gBufferTexCoord).xyz;
    vec3 normalView = texture(normalTex, gBufferTexCoord).xyz;
    
    float occlusion = float(testSampleNum);//m_ssaoSample.sampleNum;
    
    vec2 ssaoTexCoord = texCoord * tileDeminsion;//gl_FragCoord.xy;
    
    vec3 randomVec = normalize(texture(noiseTex, ssaoTexCoord)).xyz;
    vec3 tangent   = normalize(randomVec - normalView * dot(normalView, randomVec)); // X axis
    vec3 biTangent = cross(normalView, tangent);                                      // Y axis
    
    mat3 TBN = mat3(tangent, biTangent, normalView);
    
    //uint i = 0;
    for (uint i = 0; i < testSampleNum/*m_ssaoSample.sampleNum*/; ++i)
    {
        vec3 sampleVec = TBN * testSamples[i];//m_ssaoSample.samples[i];
        //vec3 samplePosWorld = posWorld + sampleVec;
        
        vec3 samplePos = posView + sampleVec;
        
        //vec4 samplePosView = viewMat * vec4(samplePosWorld, 1.0f);
        //vec3 samplePos = (samplePosView.xyz / samplePosView.w);
        
        vec4 samplePosVec4     = vec4(samplePos, 1.0f);
        vec4 samplePosProjVec4 = projMat * samplePosVec4;
        samplePosProjVec4.xy  /= samplePosProjVec4.w;
        samplePosProjVec4.xy   = 0.5f * samplePosProjVec4.xy + 0.5f;
        
        vec4 depthGPosWorld = texture(posTex, samplePosProjVec4.xy/*gBufferTexCoord*/);
        
        vec3 depthPosWorld;
        vec3 depthPosView = depthGPosWorld.xyz;
        float depth;
        
        if (depthGPosWorld.w > 0.99999f)
        {
            //depthPosWorld = depthGPosWorld.xyz;
            //depthPosView  = viewMat * vec4(depthPosWorld, 1.0f);
            //depth = depthPosView.z / depthPosView.w;
            depth = depthPosView.z;
            
            if (samplePos.z < depth + 0.0001f) // occlusion
            {
                occlusion -= 1.0f;
                //occlusion = 0.0f;
            }
        }
    }
    
    occlusion /= 64.0f;//float(/*m_ssaoSample.sampleNum*/testSampleNum);
    
    ssaoTexture = occlusion; // 0
    
    // depthTex.z = depthPosWorld.z; // 1
    ///depthTex = depthGPosWorld;
    ///depthViewTex.z = depth; // 2
    ///samplePosTex = samplePos;//vec3(samplePos.x + 5.0f, samplePos.y + 5.0f, samplePos.z); // 3
    
    //posWorldTex = posWorld.xyz; // 4
    //posViewTex = posView.xyz; // 5
    
    ///sampleVecTex = sampleVec; // 6
    //samplePosProjTex.xy = samplePosProjVec4.xy; // 7
    
    ///xAxis = tangent;
    ///yAxis = biTangent;
    ///zAxis = normalView;
}