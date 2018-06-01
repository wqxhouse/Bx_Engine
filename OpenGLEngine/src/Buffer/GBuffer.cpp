#include "GBuffer.h"

#include "../Context/Scene.h"

GBuffer::GBuffer(const UINT width, const UINT height)
    : m_gFramebuffer(5), m_width(width), m_height(height)
{

}

GBuffer::~GBuffer()
{
}

BOOL GBuffer::initialize()
{
    m_gFramebuffer.createFramebuffer();

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

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE4,
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
        printf("Fail to compile GBuffer shaders.\n");
        assert(FALSE);
    }

    return hs;
}

void GBuffer::drawGBuffer(Scene* pScene)
{
    m_gShader.useProgram();
    m_gFramebuffer.drawFramebuffer();

    size_t modelSize = pScene->GetModelSize();
    for (size_t i = 0; i < modelSize; ++i)
    {
        Model* pModel = pScene->GetModelPtr(i);

        GLint worldMatrixLocation = glGetUniformBlockIndex(m_gShader.GetShaderProgram(), "worldMatrix");

        if (worldMatrixLocation >= 0)
        {
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE,
                               glm::value_ptr(pModel->m_pTrans->GetTransMatrix()));

            pModel->updateMaterial(pScene->GetUniformBufferMgr(), pScene->GetMaterialUniformBufferIndex());
            pModel->draw();
        }
        else
        {
            printf("Can't find worldMatrix location!");
            assert(FALSE);
        }
    }

    m_gFramebuffer.finishDrawFramebuffer();
    m_gShader.finishProgram();
}
