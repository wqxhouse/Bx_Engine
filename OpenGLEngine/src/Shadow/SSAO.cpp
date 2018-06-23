#include "SSAO.h"
#include "../Context/Scene.h"
#include "../Buffer/GBuffer.h"

#define SSAO_NOISE_TEXTURE_DEMENSION 4
#define SSAO_NOISE_TEXTURE_DATA_SIZE 16

static std::default_random_engine generator;
static std::uniform_real_distribution<float> posDistribution(0.0f, 1.0f);
static std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

SSAO::SSAO(
    Scene*          pScene,
    const Setting*  pSetting)
    : m_pSetting(pSetting),
      m_pScene(pScene),
      m_pSsaoTexture(NULL),
      m_pNoiseTexture(NULL)
{
    AmbientOcclutionSetting ambientOcclution =
        m_pSetting->m_graphicsSetting.ambientOcclutionSetting;

    m_sampleNum = ambientOcclution.ssaoQuality;

    m_ssaoSampleVectors.reserve(m_sampleNum);
    m_noiseTextureData.reserve(SSAO_NOISE_TEXTURE_DATA_SIZE);
}

SSAO::~SSAO()
{
    SafeDelete(m_pNoiseTexture);
}

BOOL SSAO::initialize()
{
    BOOL result = TRUE;

    m_ssaoFramebuffer.createFramebuffer();

    // Create ssao texture and attach to framebuffer
    m_pSsaoTexture = new Texture2D(m_pSetting->width,
                                   m_pSetting->height,
                                   1,
                                   GL_RGBA,
                                   GL_R32F,
                                   GL_FLOAT,
                                   GL_CLAMP_TO_BORDER,
                                   FALSE);

    m_ssaoFramebuffer.attachTexture2D(GL_TEXTURE0, GL_COLOR_ATTACHMENT0, m_pSsaoTexture, 1);

    // Compiler and link ssao shaders
    m_ssaoShader.setShaderFiles("SSAO.vert", "SSAO.frag");
    result = m_ssaoShader.linkProgram();

    if (result == FALSE)
    {
        printf("Fail to compile SSAO shaders.\n");
        assert(FALSE);
    }

    for (size_t i = 0; i < m_sampleNum; ++i)
    {
        Math::Vector3 sample = { distribution(generator),      // x
                                 distribution(generator),      // y
                                 posDistribution(generator) }; // z

        float scale = posDistribution(generator);
        sample     *= scale;
        // TODO: Added lerp here

        m_ssaoSampleVectors.push_back(sample);
    }

    /*UniformBufferMgr* pUboMgr = m_pScene->GetUniformBufferMgr();

    m_sampleUboIndex = pUboMgr->createUniformBuffer(
                                  GL_STATIC_DRAW,
                                  GetSsaoSampleDataSize(),
                                  GetSsaoUboData());

    pUboMgr->bindUniformBuffer(m_sampleUboIndex,
                               m_ssaoShader.GetShaderProgram(),
                               "SsaoSamplesUniformBlock");*/

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
                                    GL_RGB32F,
                                    GL_RGB32F,
                                    GL_FLOAT,
                                    GL_REPEAT,
                                    FALSE,
                                    m_noiseTextureData.data());

    m_ssaoQuad.initialize();

    return result;
}

void SSAO::draw()
{
    GLuint ssaoShaderProgram = m_ssaoShader.useProgram();

    GBuffer* pGBuffer = m_pScene->GetGBuffer();

    pGBuffer->readGBuffer(ssaoShaderProgram, "posTex", GL_TEXTURE0);
    pGBuffer->readGBuffer(ssaoShaderProgram, "normalTex", GL_TEXTURE1);
    pGBuffer->readGBuffer(ssaoShaderProgram, "texCoordTex", GL_TEXTURE2);

    GLint viewMatLocation = glGetUniformLocation(ssaoShaderProgram, "viewMat");
    GLint projMatLocation = glGetUniformLocation(ssaoShaderProgram, "projMat");

    if (viewMatLocation >= 0 && projMatLocation >= 0)
    {
        Camera* pActiveCam = m_pScene->GetActivateCamera();

        const GLfloat* viewMatData = glm::value_ptr(pActiveCam->GetViewMatrix());
        glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, viewMatData);

        const GLfloat* projMatData = glm::value_ptr(pActiveCam->GetProjectionMatrix());
        glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, projMatData);
    }
    else
    {
        printf("Unable to get projection matrix location in shader");
        //assert(FALSE);
    }

    // Test
    GLint sampleNumLocation = glGetUniformLocation(ssaoShaderProgram, "testSampleNum");
    glUniform1ui(sampleNumLocation, /*static_cast<GLuint>(m_sampleNum)*/64u);

    GLint sampleLocation = glGetUniformLocation(ssaoShaderProgram, "testSamples");
    glUniform3fv(sampleLocation, m_sampleNum, (const GLfloat*)(m_ssaoSampleVectors.data()));
    // Test end

    m_ssaoFramebuffer.drawFramebuffer();
    m_pNoiseTexture->bindTexture(GL_TEXTURE3, ssaoShaderProgram, "noiseTex", 3);
    m_ssaoQuad.draw();
    m_ssaoFramebuffer.finishDrawFramebuffer();

    m_ssaoShader.FinishProgram();
}
