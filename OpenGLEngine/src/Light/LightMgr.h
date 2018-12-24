//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "Light.h"

#include "../Buffer/UniformBufferMgr.h"
#include "../Shadow/ShadowMgr.h"

#define MAX_LIGHT_UBO_NUM 256
#define MAX_LIGHT_NUM     4096

class LightMgr
{
public:
    LightMgr(Scene* pScene);
    ~LightMgr();

    BOOL initialize();
    void castShadow();

    void updateLightUbo(
        const GLuint      shaderProgram,
        const GLchar*     uniformBufferBlockName);

    void updateLightSsbo();

    void updateLightShadow();

    void bindLightUbo(
        UniformBufferMgr* m_pUboMgr,
        const GLuint      shaderProgram,
        const GLchar*     uniformBufferBlockName);

    void translateLight(
        const UINT           index,
        const Math::Vector3& transVector);

    void rotateLight(
        const UINT           index,
        const Math::Vector3& axis,
        const float          angle);

    void addDirectionalLight(
        const Math::Vector3& direction,
        const Math::Vector3& color);

    void addPointLight(
        const Math::Vector3& position,
        const Math::Vector3& color,
        const float radius);

    void addSpotLight(
        const Math::Vector3& position,
        const Math::Vector3& direction,
        const Math::Vector3& color,
        const float          distance,
        const float          in_angle,
        const float          out_angle);

    inline GLuint GetLightDataHandle() const { return m_lightUboHandle; }

    inline UINT GetLightCount() const { return m_lightList.size() - 1; }

    inline UINT GetLightDataSize() const { return m_lightList.size() * sizeof(LightData); }

    inline Light* GetLight(const UINT index)
    {
        assert(index < m_lightList.size());

        return reinterpret_cast<Light*>(&(m_lightList[index + 1]));
    }

    inline void* GetLightData() const
    { 
        return reinterpret_cast<void*>(const_cast<LightData*>(m_lightList.data()));
    }

    inline ShadowMgr* GetShadowMgr() { return &m_shadowMgr; }

private:
    void createLightUbo(UniformBufferMgr* m_pUboMgr);
    void createLightSsbo();

    struct LightData
    {
        Math::Vector4 lightBaseData[2];
        Math::Mat4    m_lightTransVP;
        Math::Vector4 lightData[2];
    };

    Scene*                 m_pScene; // Pointer to the scene

    GLuint                 m_lightUboHandle;
    std::vector<LightData> m_lightList;

    ShadowMgr m_shadowMgr;

    union LightFlags
    {
        UINT flags;

        struct
        {
            UINT isRecreateLightUbo : 1;
            UINT isUpdateLightUbo   : 1;
            UINT isUpdateLightSsbo  : 1;
            UINT reserve            : 29;
        } bitField;
    }m_lightFlags;
};