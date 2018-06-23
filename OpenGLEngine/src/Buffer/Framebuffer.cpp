#include "Framebuffer.h"

Framebuffer::Framebuffer(const UINT attachTexCount)
{
    m_pAttachedTextures.resize(attachTexCount, NULL);
}

Framebuffer::~Framebuffer()
{
    for (Texture* pTexture : m_pAttachedTextures)
    {
        SafeDelete(pTexture);
    }

    glDeleteRenderbuffers(1, &m_depthRenderBufferHandle);
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
    const UINT   samples,
    const GLenum loadFormat,
    const GLenum storeFormat,
    const GLenum texDataType,
    const GLenum wrapMethod,
    const BOOL   mipmap)
{
    Texture2D* pTexture2D = new Texture2D(
            texWidth, texHeight, samples, loadFormat, storeFormat, texDataType, wrapMethod, mipmap);

    UINT texIndex = getTextureIndex(texUnit);

    m_pAttachedTextures[texIndex] = pTexture2D;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);

    if (samples < 2)
    {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, pTexture2D->GetTextureHandle(), 0);
    }
    else
    {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D_MULTISAMPLE, pTexture2D->GetTextureHandle(), 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_framebufferAttachmentsList.push_back(attachmentType);
}

void Framebuffer::createRenderbufferAttachment(
    const FboRenderbufferAttachmentType attachmentType,
    const UINT                          m_depthBufWidth,
    const UINT                          m_depthBufHeight)
{
    GLenum attachment;
    GLenum attachmentFormat;

    switch (attachmentType)
    {
        case FRAMEBUFFER_COLOR_RENDERBUFFER_ATTACHMENT:
            // TODO
            break;

        case FRAMEBUFFER_DEPTH_RENDERBUFFER_ATTACHMENT:
            attachment       = GL_DEPTH_ATTACHMENT;
            attachmentFormat = GL_DEPTH_COMPONENT;
            break;

        case FRAMEBUFFER_STENCIL_RENDERBUFFER_ATTACHMENT:
            attachment       = GL_STENCIL_ATTACHMENT;
            attachmentFormat = GL_STENCIL_COMPONENTS;
            break;

        case FRAMEBUFFER_DEPTH_STENCIL_RENDERBUFFER_ATTACHMENT:
            attachment       = GL_DEPTH_STENCIL_ATTACHMENT;
            attachmentFormat = GL_DEPTH24_STENCIL8; // Is this correct? Need varify.
            break;

        default:
            printf("Unsupported render buffer type!");
            assert(FALSE);
    }

    glGenRenderbuffers(1, &m_depthRenderBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBufferHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, attachmentFormat, m_depthBufWidth, m_depthBufHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_depthRenderBufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTexture2D(
    const GLenum texUnit,
    const GLenum attachmentType,
    Texture2D*   pTexture2D,
    const UINT   samples)
{
    UINT texIndex = getTextureIndex(texUnit);

    m_pAttachedTextures[texIndex] = pTexture2D;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);

    if (samples < 2)
    {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, pTexture2D->GetTextureHandle(), 0);
    }
    else
    {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D_MULTISAMPLE, pTexture2D->GetTextureHandle(), 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_framebufferAttachmentsList.push_back(attachmentType);
}

void Framebuffer::drawFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);
    glDrawBuffers(m_framebufferAttachmentsList.size(), m_framebufferAttachmentsList.data());
}

void Framebuffer::readFramebuffer(
    const GLenum texUnit,
    const GLenum texType)
{
    Texture* pTexture = getTexturePtr(texUnit);

    glActiveTexture(texUnit);
    glBindTexture(texType, pTexture->GetTextureHandle());
}

void Framebuffer::finishDrawFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        printf("Invalid texture unit!\n");
        assert(FALSE);
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
