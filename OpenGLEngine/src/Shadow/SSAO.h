#pragma once

#include <random>

#include "../Context/Setting.h"
#include "../Math/Vector4.h"
#include "../Texture/Texture.h"
#include "../Buffer/UniformBufferMgr.h"
#include "../Shader/Shader.h"
#include "../Model/Primitives/Quad.h"
#include "../Effect/BlurEffect.h"

#define SSAO_SAMPLE_MAX 256

class Scene;

class SSAO
{
public:
    SSAO(
        Scene*          pScene,
        const Setting*  pSetting);

    ~SSAO();

    BOOL initialize();
    void draw();

    inline Texture2D* GetSsaoTexture()    const { return m_pSsaoTexture;   }
    inline UINT       GetSampleNum()      const { return m_sampleNum;      }
    inline UINT       GetSampleUboIndex() const { return m_sampleUboIndex; }

    inline UINT  GetSsaoSampleDataSize()
    {
        return (sizeof(m_sampleNumVec4) + sizeof(m_ssaoSamples[0]) * m_sampleNum);
    }

    inline void* GetSsaoSampleData() { return &(m_ssaoSamples[0]); }
    inline void* GetSsaoUboData()    { return static_cast<void*>(&m_sampleNum); }

    void bindSsaoTexture(
        const GLenum       texUnit,
        const UINT         program,
        const std::string& texName,
        const UINT         samplerIndex);

    void unbindSsaoTexture();

private:
    const Setting* m_pSetting;

    Scene* m_pScene;

    Framebuffer m_ssaoFramebuffer;
    Texture2D* m_pSsaoTexture;

    UINT m_sampleUboIndex;

    Texture2D* m_pNoiseTexture;
    std::vector<Math::Vector3> m_noiseTextureData;

    GraphicsShader m_ssaoShader;

    Quad m_ssaoQuad;

    BOOL blurSSAO;
    BlurEffect* m_pBlurEffect;

    union
    {
        struct
        {
            UINT m_sampleNum; Math::Vector3 padding;
        };
        Math::Vector4 m_sampleNumVec4;
    };
    Math::Vector4 m_ssaoSamples[SSAO_SAMPLE_MAX];
};