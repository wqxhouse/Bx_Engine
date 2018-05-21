#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Texture/Texture.h"

class Framebuffer
{
public:
    Framebuffer(const UINT attachTexCount = 1);
    virtual ~Framebuffer();

    void createFramebufferTexture2D(
        const GLenum texUnit,
        const GLenum attachmentType,
        const UINT   texWidth,
        const UINT   texHeight,
        const GLenum format,
        const GLenum texDataType,
        const GLenum wrapMethod = GL_CLAMP,
        const BOOL   mipmap     = GL_FALSE);

    void drawFramebuffer();
    void readFramebuffer(
        const GLenum texUnit,
        const GLenum texType);

    UINT getTextureIndex(const GLenum texUnit) const;
    Texture* getTexturePtr(const GLenum texUnit) const;

    inline GLuint getFramebufferHandle() const { return m_framebufferHandle; }

private:
    BOOL checkFramebufferStatus() const;

    GLuint m_framebufferHandle;

    std::vector<Texture*> m_pAttachedTextures;
};

class DepthFramebuffer : public Framebuffer
{
public:
    DepthFramebuffer(const UINT depthTexWidth, const UINT depthTexHeight);
    ~DepthFramebuffer();
};