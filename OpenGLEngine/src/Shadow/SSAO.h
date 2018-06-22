#pragma once

#include <random>

#include "../Context/Setting.h"
#include "../Math/Vector3.h"
#include "../Texture/Texture.h"
#include "../Buffer/UniformBufferMgr.h"

class SSAO
{
public:
    SSAO(
        const Setting*          pSetting,
        UniformBufferMgr*       pUboMgr);
    ~SSAO();

    void initialize();
    void draw(const UINT shaderProgram);

    inline Texture2D* GetSsaoTexture()    const { return m_pSsaoTexture;   }
    inline UINT       GetSampleNum()      const { return m_sampleNum;      }
    inline UINT       GetSampleUboIndex() const { return m_sampleUboIndex; }

    inline UINT  GetSsaoSampleDataSize()
    {
        return (sizeof(m_sampleNum) + sizeof(Math::Vector3) * m_sampleNum);
    }

    inline void* GetSsaoSampleData() { return m_ssaoSampleVectors.data(); }
    inline void* GetSsaoUboData() { return static_cast<void*>(&m_sampleNum); }

private:
    const Setting* m_pSetting;

    Texture2D* m_pSsaoTexture;

    UniformBufferMgr*          m_pUboMgr;
    UINT                       m_sampleUboIndex;

    Texture2D* m_pNoiseTexture;
    std::vector<Math::Vector3> m_noiseTextureData;

    UINT m_sampleNum; Math::Vector3 padding;
    std::vector<Math::Vector3> m_ssaoSampleVectors;
};