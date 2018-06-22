#include "SSAO.h"

#define SSAO_NOISE_TEXTURE_DEMENSION 4
#define SSAO_NOISE_TEXTURE_DATA_SIZE 16

static std::default_random_engine generator;
static std::uniform_real_distribution<float> posDistribution(0.0f, 1.0f);
static std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

SSAO::SSAO(
    const Setting*          pSetting,
    UniformBufferMgr*       pUboMgr)
    : m_pSetting(pSetting),
      m_pUboMgr(pUboMgr),
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
    SafeDelete(m_pSsaoTexture);
    SafeDelete(m_pNoiseTexture);
}

void SSAO::initialize()
{
    m_pSsaoTexture = new Texture2D(m_pSetting->width,
                                   m_pSetting->height,
                                   1,
                                   GL_RGB32F,
                                   GL_RGB32F,
                                   GL_FLOAT,
                                   GL_CLAMP_TO_BORDER,
                                   FALSE);

    for (size_t i = 0; i < m_sampleNum; ++i)
    {
        Math::Vector3 sample = { distribution(generator),      // x
                                 distribution(generator),      // y
                                 posDistribution(generator) }; // z

        m_ssaoSampleVectors.push_back(sample);
    }

    m_sampleUboIndex = m_pUboMgr->createUniformBuffer(
                                  GL_STATIC_DRAW,
                                  GetSsaoSampleDataSize(),
                                  GetSsaoUboData());

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
}

void SSAO::draw(const UINT shaderProgram)
{
    m_pNoiseTexture->bindTexture(GL_TEXTURE6, shaderProgram, "noiseTex", 6);
}
