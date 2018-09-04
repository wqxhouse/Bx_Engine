#pragma once

#include "Light.h"

#include "../Buffer/UniformBufferMgr.h"
#include "../Shadow/ShadowMgr.h"

#define MAX_LIGHT_NUM 32

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

    inline UINT GetLightCount() const { return m_lightList.size(); }

    inline UINT GetLightDataSize() const { return GetLightCount() * sizeof(LightData); }

    inline Light* GetLight(const UINT index)
    {
        return reinterpret_cast<Light*>(&(m_lightList[index]));
    }

    inline void* GetLightData() const
    { 
        return reinterpret_cast<void*>(const_cast<LightData*>(m_lightList.data()));
    }

    inline ShadowMgr* GetShadowMgr() { return &m_shadowMgr; }

private:
    void createLightUbo(UniformBufferMgr* m_pUboMgr);

    struct LightData
    {
        Math::Vector4 data[4];
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
            UINT reserve            : 30;
        } bitField;
    }m_lightFlags;
};