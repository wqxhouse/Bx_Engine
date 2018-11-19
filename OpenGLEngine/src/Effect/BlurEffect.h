//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Buffer/Framebuffer.h"
#include "../Shader/GraphicsShader.h"
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
    Texture2D*     m_pInputTexture;

    GraphicsShader m_blurShader;
    Framebuffer    m_blurFbo;

    Quad*          m_pBlurQuad;
};