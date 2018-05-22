#include "Framebuffer.h"

DepthFramebuffer::DepthFramebuffer()
    : Framebuffer()
{ }

DepthFramebuffer::~DepthFramebuffer()
{ }

void DepthFramebuffer::createFramebuffer(
    const UINT depthTexWidth,
    const UINT depthTexHeight)
{
    glGenFramebuffers(1, &m_framebufferHandle); 
    
    createFramebufferTexture2D(
        GL_TEXTURE0,
        GL_DEPTH_ATTACHMENT,
        depthTexWidth, depthTexHeight,
        GL_DEPTH_COMPONENT,
        GL_FLOAT);

    glBindFramebuffer(GL_FRAMEBUFFER, getFramebufferHandle());

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthFramebuffer::finishDrawFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDrawBuffer(GL_FRONT);
    glReadBuffer(GL_FRONT);
}
