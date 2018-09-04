#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Texture/Texture.h"

#include <unordered_set>

enum FboRenderbufferAttachmentType
{
    FRAMEBUFFER_COLOR_RENDERBUFFER_ATTACHMENT,
    FRAMEBUFFER_DEPTH_RENDERBUFFER_ATTACHMENT,
    FRAMEBUFFER_STENCIL_RENDERBUFFER_ATTACHMENT,
    FRAMEBUFFER_DEPTH_STENCIL_RENDERBUFFER_ATTACHMENT,
};

class Framebuffer final
{
public:
    Framebuffer(const UINT attachTexCount = 1);
    ~Framebuffer();

    void createFramebufferTexture2D(
        const GLenum texUnit,
        const GLenum attachmentType,
        const UINT   texWidth,
        const UINT   texHeight,
        const UINT   samples,
        const GLenum loadFormat,
        const GLenum storeFormat,
        const GLenum texDataType,
        const GLenum wrapMethod = GL_CLAMP,
        const BOOL   mipmap     = GL_FALSE);

    /*void createFramebufferTexture3D(
        const GLenum texUnit,
        const GLenum attachmentType,
        const UINT   texWidth,
        const UINT   texHeight,
        const UINT   layers,
        const UINT   samples,
        const GLenum loadFormat,
        const GLenum storeFormat,
        const GLenum texDataType,
        const GLenum wrapMethod = GL_CLAMP,
        const BOOL   mipmap     = GL_FALSE);*/

    void createRenderbufferAttachment(
        const FboRenderbufferAttachmentType attachmentType,
        const UINT                          m_depthBufWidth,
        const UINT                          m_depthBufHeight);

    void attachTexture2D(
        const GLenum texUnit,
        const GLenum attachmentType,
        Texture2D*   pTexture2D,
        const UINT   samples);

    void attachTexture3D(
        const GLenum texUnit,
        const GLenum attachmentType,
        Texture3D*   pTexture3D,
        const UINT   layer);

    void attachCubemap(
        const GLenum texUnit,
        const GLenum attachmentType,
        Cubemap*     pCubemap,
        const GLenum cubeface,
        const BOOL   addDrawBuffer = FALSE);

    void drawFramebuffer();
    void readFramebuffer(
        const GLenum texUnit,
        const GLenum texType);

    virtual void finishDrawFramebuffer();

    UINT getTextureIndex(const GLenum texUnit) const;
    Texture* getTexturePtr(const GLenum texUnit) const;

    inline GLuint getFramebufferHandle() const { return m_framebufferHandle; }

protected:
    GLuint m_framebufferHandle;
    GLuint m_depthRenderBufferHandle;

private:
    BOOL checkFramebufferStatus() const;

    std::vector<Texture*> m_pAttachedTextures;
    std::vector<GLenum>   m_framebufferAttachmentsList;

    // TODO: Optimize
    std::unordered_set<GLenum> m_framebufferAttachmentsSet;
};
