#include "Framebuffer.h"

DepthFramebuffer::DepthFramebuffer(
    const UINT depthTexWidth,
    const UINT depthTexHeight)
{
    createFramebufferTexture2D(GL_TEXTURE0,
                               GL_DEPTH_ATTACHMENT,
                               depthTexWidth, depthTexHeight,
                               GL_DEPTH_COMPONENT,
                               GL_FLOAT);

    glBindFramebuffer(GL_FRAMEBUFFER, getFramebufferHandle());

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthFramebuffer::~DepthFramebuffer()
{
}
