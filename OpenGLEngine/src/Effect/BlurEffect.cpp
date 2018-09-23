#include "BlurEffect.h"

BlurEffect::BlurEffect(
    Texture2D* pInputTexture)
    : m_pInputTexture(pInputTexture)
{
}

BlurEffect::~BlurEffect()
{
    SafeDelete(m_pBlurQuad);
}

BOOL BlurEffect::initialize()
{
    BOOL result = TRUE;

    m_blurShader.setShaderFiles("TextureBlur.vert", "TextureBlur.frag");
    result = m_blurShader.linkProgram();

    assert(result == TRUE);

    m_blurFbo.createFramebufferTexture2D(GL_TEXTURE0,
                                         GL_COLOR_ATTACHMENT0,
                                         GL_COLOR_ATTACHMENT0,
                                         m_pInputTexture->GetTextureWidth(),
                                         m_pInputTexture->GetTextureHeight(),
                                         1,
                                         GL_RGB,
                                         GL_RGB32F,
                                         GL_FLOAT,
                                         GL_CLAMP_TO_BORDER,
                                         FALSE);

    m_pBlurQuad = new Quad();
    m_pBlurQuad->initialize();

    return result;
}

void BlurEffect::draw()
{
    GLint blurShader = m_blurShader.useProgram();

    m_blurFbo.setRenderTargets();
    
    m_pInputTexture->bindTexture(GL_TEXTURE0, blurShader, "inputTexture");

    m_pBlurQuad->draw();

    m_blurFbo.finishDrawFramebuffer();

    m_blurShader.FinishProgram();
}
