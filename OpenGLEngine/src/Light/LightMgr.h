#pragma once

#include "Light.h"

#include "../Buffer/UniformBufferMgr.h"

class LightMgr
{
public:
    LightMgr();
    ~LightMgr();

    void createLightUbo(UniformBufferMgr* m_pUboMgr);

    void updateLightUbo(UniformBufferMgr* m_pUboMgr);

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
        float radius);

    void addSpotLight(
        const Math::Vector3& position,
        const Math::Vector3& color,
        float radius_in,
        float radius_out);

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

private:
    struct LightData
    {
        Math::Vector4 data[3];
    };

    GLuint                 m_lightUboHandle;
    std::vector<LightData> m_lightList;
};