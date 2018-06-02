#pragma once

#include "Framebuffer.h"
#include "../Shader/Shader.h"

class Scene;

class GBuffer
{
public:
    GBuffer(
        Scene*       pScene,
        const UINT   width,
        const UINT   height);
    ~GBuffer();

    BOOL initialize();
    void drawGBuffer();

private:
    Scene*      m_pScene; // Pointer to the scene

    Framebuffer m_gFramebuffer;
    Shader      m_gShader;

    UINT m_width;
    UINT m_height;
};