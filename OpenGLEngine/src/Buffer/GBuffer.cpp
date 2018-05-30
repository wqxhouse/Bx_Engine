#include "GBuffer.h"

#include "../Context/Scene.h"

GBuffer::GBuffer(const UINT width, const UINT height)
    : m_gFramebuffer(4), m_width(width), m_height(height)
{

}

GBuffer::~GBuffer()
{
}

BOOL GBuffer::initialize()
{
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE0,
                                              GL_COLOR_ATTACHMENT0,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_COLOR_ATTACHMENT0,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE1,
                                              GL_COLOR_ATTACHMENT1,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_COLOR_ATTACHMENT1,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE2,
                                              GL_COLOR_ATTACHMENT2,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_COLOR_ATTACHMENT2,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE3,
                                              GL_COLOR_ATTACHMENT3,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_COLOR_ATTACHMENT3,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gShader.setShaderFiles("GBuffer.vert", "GBuffer.frag");
    BOOL hs = m_gShader.linkProgram();

    if (hs == FALSE)
    {
        assert("Fail to compile GBuffer shaders.\n");
    }

    return hs;
}

void GBuffer::drawGBuffer(Scene* pScene)
{
    m_gShader.useProgram();

    size_t modelSize = pScene->GetModelSize();
    for (size_t i = 0; i < modelSize; ++i)
    {
        pScene->GetModelPtr(i)->draw();
    }

    m_gShader.finishProgram();
}
