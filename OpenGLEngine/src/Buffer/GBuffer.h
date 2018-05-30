#pragma once

#include "Framebuffer.h"
#include "../Shader/Shader.h"

class Scene;

class GBuffer
{
public:
    GBuffer(const UINT width, const UINT height);
    ~GBuffer();

    BOOL initialize();
    void drawGBuffer(Scene* pScene);

private:
    Framebuffer m_gFramebuffer;
    Shader      m_gShader;

    UINT m_width;
    UINT m_height;
};