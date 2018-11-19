//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Texture/Texture.h"
#include "../Camera/Camera.h"
#include "../Shader/Shader.h"
#include "../Buffer/Framebuffer.h"

#define LIGHT_PROBE_Z_POS   0x0
#define LIGHT_PROBE_Z_NAG   0x1
#define LIGHT_PROBE_Y_POS   0x2
#define LIGHT_PROBE_Y_NAG   0x3
#define LIGHT_PROBE_X_POS   0x4
#define LIGHT_PROBE_X_NAG   0x5

class Scene;

class LightProbe
{
public:
    LightProbe(
        Scene*               pScene,
        const Math::Vector3& pos,
        const float          nearClip,
        const float          farClip);
    ~LightProbe();

    BOOL initialize();

    void update(
        const Math::Vector3& pos);

    void draw();

    void readLightProbe(
        const GLuint       shaderProgram,
        const std::string& textureName,
        const GLenum       texUnit);

    inline BOOL IsFirstDraw() const { return firstDraw; }

    inline void SetProbeResolution(const UINT resolution) { m_probeResolution = resolution; }

    ProspectiveCamera* m_pCubemapCam[CUBE_MAP_FACE_NUM];

private:
    Scene*   m_pScene; // Pointer to the scene

    Cubemap* m_pCubemap;

    Framebuffer m_probeFbo;

    UINT m_probeResolution;

    Math::Vector3 m_pos;
    float         m_nearClip;
    float         m_farClip;

    BOOL firstDraw;
};