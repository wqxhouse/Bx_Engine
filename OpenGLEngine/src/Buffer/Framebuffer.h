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
#include "../Texture/Texture.h"

#include <unordered_set>

class Framebuffer final
{
public:
    Framebuffer(const UINT attachTexCount = 1);
    ~Framebuffer();

    void createFramebufferTexture2D(
        const GLenum texUnit,
        const GLenum attachmentType,
        const GLenum renderTarget,
        const UINT   texWidth,
        const UINT   texHeight,
        const UINT   samples,
        const GLenum loadFormat,
        const GLenum storeFormat,
        const GLenum texDataType,
        const GLenum wrapMethod = GL_CLAMP,
        const BOOL   mipmap     = GL_FALSE);

    void createRenderbufferAttachment(
        const FboRenderbufferAttachmentType attachmentType,
        const UINT                          m_depthBufWidth,
        const UINT                          m_depthBufHeight);

    void attachTexture2D(
        const GLenum texUnit,
        const GLenum attachmentType,
        Texture2D*   pTexture2D,
        const UINT   samples,
        const GLenum renderTarget);

    void attachTexture3D(
        const GLenum texUnit,
        const GLenum attachmentType,
        Texture3D*   pTexture3D,
        const UINT   layer,
        const GLenum renderTarget);

    void attachCubemap(
        const GLenum texUnit,
        const GLenum attachmentType,
        Cubemap*     pCubemap,
        const GLenum cubeface,
        const GLenum renderTarget);

    void setRenderTargets();
    void setRenderTargets(const std::vector<GLenum>& renderTargets);

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
