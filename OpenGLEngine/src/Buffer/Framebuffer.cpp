#include "Framebuffer.h"

Framebuffer::Framebuffer(const UINT attachTexCount)
{
    m_pAttachedTextures.resize(attachTexCount);
}

Framebuffer::~Framebuffer()
{
    for (Texture* pTexture : m_pAttachedTextures)
    {
        SafeDelete(pTexture);
    }

    glDeleteFramebuffers(1, &m_framebufferHandle);
}

void Framebuffer::createFramebuffer()
{
    glGenFramebuffers(1, &m_framebufferHandle);
}

void Framebuffer::createFramebufferTexture2D(
    const GLenum texUnit,
    const GLenum attachmentType,
    const UINT   texWidth,
    const UINT   texHeight,
    const GLenum format,
    const GLenum texDataType,
    const GLenum wrapMethod,
    const BOOL   mipmap)
{
    Texture2D* pTexture2D =
        new Texture2D(texWidth, texHeight, format, texDataType, wrapMethod, mipmap);

    UINT texIndex = getTextureIndex(texUnit);

    m_pAttachedTextures[texIndex] = pTexture2D;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, pTexture2D->GetTextureHandle(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::drawFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);
}

void Framebuffer::readFramebuffer(
    const GLenum texUnit,
    const GLenum texType)
{
    Texture* pTexture = getTexturePtr(texUnit);

    glActiveTexture(texUnit);
    glBindTexture(texType, pTexture->GetTextureHandle());
}

UINT Framebuffer::getTextureIndex(const GLenum texUnit) const
{
    assert(texUnit >= GL_TEXTURE0 && texUnit <= GL_MAX_TEXTURE_UNITS);

    return texUnit - GL_TEXTURE0;
}

Texture* Framebuffer::getTexturePtr(const GLenum texUnit) const
{
    Texture* pTexture = NULL;
    
    UINT texIndex = getTextureIndex(texUnit);

    if (texIndex >= 0 && texUnit - GL_TEXTURE0 < m_pAttachedTextures.size())
    {
        pTexture = m_pAttachedTextures[texIndex];
    }
    else
    {
        assert("Invalid texture unit!");
    }

    return pTexture;
}

BOOL Framebuffer::checkFramebufferStatus() const
{
    BOOL status = TRUE;

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer error: 0x%x\n", framebufferStatus);

        status = FALSE;
    }

    return status;
}
