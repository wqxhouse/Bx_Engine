#include "SSAO.h"
#include "../Context/Scene.h"
#include "../Buffer/GBuffer.h"

#define SSAO_NOISE_TEXTURE_DEMENSION 4
#define SSAO_NOISE_TEXTURE_DATA_SIZE 16

static std::random_device rd;
static std::default_random_engine generator(rd());
static std::uniform_real_distribution<float> distribution(0.1f, 1.0f);

SSAO::SSAO(
    Scene*          pScene,
    const Setting*  pSetting)
    : m_pSetting(pSetting),
      m_pScene(pScene),
      m_pSsaoTexture(NULL),
      m_pNoiseTexture(NULL),
      blurSSAO(TRUE)
{
    AmbientOcclutionSetting ambientOcclution =
        m_pSetting->m_graphicsSetting.ambientOcclutionSetting;

    m_sampleNum = ambientOcclution.ssaoQuality;

    m_noiseTextureData.reserve(SSAO_NOISE_TEXTURE_DATA_SIZE);
}

SSAO::~SSAO()
{
    SafeDelete(m_pNoiseTexture);
    SafeDelete(m_pBlurEffect);
}

BOOL SSAO::initialize()
{
    std::vector<Math::Vector4> m_ssaoSampleVectors(m_sampleNum, Vector4());

    BOOL result = TRUE;

    // Create ssao texture and attach to framebuffer
    m_pSsaoTexture = new Texture2D(m_pSetting->resolution.width,
                                   m_pSetting->resolution.height,
                                   1,
                                   GL_RGB,
                                   GL_RGB32F,
                                   GL_FLOAT,
                                   GL_CLAMP_TO_BORDER,
                                   FALSE);

    m_ssaoFramebuffer.attachTexture2D(GL_TEXTURE0, GL_COLOR_ATTACHMENT0, m_pSsaoTexture, 1);

    // Compiler and link ssao shaders
    m_ssaoShader.setShaderFiles("SSAO.vert", "SSAO.frag");
    result = m_ssaoShader.linkProgram();

    if (result == FALSE)
    {
        Shader::AssertErrors("Fail to compile SSAO shaders.\n");
    }

    for (size_t i = 0; i < m_sampleNum; ++i)
    {
        Math::Vector3 sample = { distribution(generator),   // x
                                 distribution(generator),   // y
                                 distribution(generator) }; // z

        // Map to [-1, 1]
        sample.x = (sample.x - 0.5f) * 2.0f;
        sample.y = (sample.y - 0.5f) * 2.0f;

        float scale = distribution(generator);
        sample     *= scale;
        // TODO: Added lerp here

        m_ssaoSampleVectors[i] = Math::Vector4(sample, 1.0f);
    }

    memcpy(m_ssaoSamples, m_ssaoSampleVectors.data(), m_sampleNum * sizeof(Math::Vector4));

    UniformBufferMgr* pUboMgr = m_pScene->GetUniformBufferMgr();

    m_sampleUboIndex = pUboMgr->createUniformBuffer(
                                  GL_STATIC_DRAW,
                                  GetSsaoSampleDataSize(),
                                  GetSsaoUboData());

    pUboMgr->bindUniformBuffer(m_sampleUboIndex,
                               m_ssaoShader.GetShaderProgram(),
                               "SsaoSamplesUniformBlock");

    for (UINT i = 0; i < SSAO_NOISE_TEXTURE_DATA_SIZE; ++i)
    {
        Math::Vector3 noiseVec = { distribution(generator), // x
                                   distribution(generator), // y
                                   0.0f };                  // z

        m_noiseTextureData.push_back(noiseVec);
    }

    m_pNoiseTexture = new Texture2D(SSAO_NOISE_TEXTURE_DEMENSION,
                                    SSAO_NOISE_TEXTURE_DEMENSION,
                                    1,
                                    GL_RGB,
                                    GL_RGB32F,
                                    GL_FLOAT,
                                    GL_REPEAT,
                                    FALSE,
                                    m_noiseTextureData.data());

    m_ssaoQuad.initialize();

    m_pBlurEffect = new BlurEffect(m_pSsaoTexture);
    result = m_pBlurEffect->initialize();
    if (result == FALSE)
    {
        printf("Fail to initialize blur effect.\n");
        assert(FALSE);
    }

    return result;
}

void SSAO::draw()
{
    GLuint ssaoShaderProgram = m_ssaoShader.useProgram();

    GBuffer* pGBuffer = m_pScene->GetGBuffer();

    pGBuffer->readGBuffer(ssaoShaderProgram, "posTex", GL_TEXTURE6);
    pGBuffer->readGBuffer(ssaoShaderProgram, "normalTex", GL_TEXTURE1);
    pGBuffer->readGBuffer(ssaoShaderProgram, "texCoordTex", GL_TEXTURE2);

    GLint projMatLocation = glGetUniformLocation(ssaoShaderProgram, "projMat");

    if (projMatLocation >= 0)
    {
        Camera* pActiveCam = m_pScene->GetActivateCamera();

        const GLfloat* projMatData = glm::value_ptr(pActiveCam->GetProjectionMatrix());
        glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, projMatData);
    }
    else
    {
        printf("Unable to get projection matrix location in shader\n");
        assert(FALSE);
    }

    m_ssaoFramebuffer.drawFramebuffer();
    m_pNoiseTexture->bindTexture(GL_TEXTURE3, ssaoShaderProgram, "noiseTex", 3);
    m_ssaoQuad.draw();
    m_ssaoFramebuffer.finishDrawFramebuffer();

    m_ssaoShader.FinishProgram();

    if (blurSSAO == TRUE)
    {
        m_pBlurEffect->draw();
    }
}

// Bind the ssao texture to program for final rendering to backbuffer
void SSAO::bindSsaoTexture(
    const GLenum       texUnit,
    const UINT         program,
    const std::string& texName,
    const UINT         samplerIndex)
{
    Texture2D* ssaoTexture = ((blurSSAO == TRUE) ? m_pBlurEffect->GetBlurTexture() : m_pSsaoTexture);
    
    ssaoTexture->bindTexture(texUnit, program, texName, samplerIndex);
}

void SSAO::unbindSsaoTexture()
{
    // TODO: unbind ssao texture
}
