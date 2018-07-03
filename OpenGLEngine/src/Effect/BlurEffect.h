#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Buffer/Framebuffer.h"
#include "../Shader/Shader.h"
#include "../Model/Primitives/Quad.h"

class BlurEffect
{
public:
    BlurEffect(
        Texture2D* pInputTexture);

    ~BlurEffect();

    BOOL initialize();
    void draw();

    inline Texture2D* GetBlurTexture() const
    {
        return static_cast<Texture2D*>(m_blurFbo.getTexturePtr(GL_TEXTURE0));
    }

private:
    Texture2D*  m_pInputTexture;

    Shader      m_blurShader;
    Framebuffer m_blurFbo;

    Quad        m_blurQuad;
};