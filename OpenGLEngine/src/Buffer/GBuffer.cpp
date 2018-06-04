#include "GBuffer.h"

#include "../Context/Scene.h"

GBuffer::GBuffer(
    Scene*       pScene,
    const UINT   width,
    const UINT   height)
    : m_gFramebuffer(5),
      m_pScene(pScene),
      m_width(width),
      m_height(height)
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
                                              GL_RGBA,
                                              GL_RGBA32F,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE1,
                                              GL_COLOR_ATTACHMENT1,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA32F,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE2,
                                              GL_COLOR_ATTACHMENT2,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE3,
                                              GL_COLOR_ATTACHMENT3,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE4,
                                              GL_COLOR_ATTACHMENT4,
                                              m_width, 
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gFramebuffer.createRenderbufferAttachment(
        FRAMEBUFFER_DEPTH_RENDERBUFFER_ATTACHMENT, m_width, m_height);

    m_gShader.setShaderFiles("GBuffer.vert", "GBuffer.frag");
    BOOL hs = m_gShader.linkProgram();

    if (hs == FALSE)
    {
        printf("Fail to compile GBuffer shaders.\n");
        assert(FALSE);
    }

    m_pScene->GetUniformBufferMgr()->bindUniformBuffer(
        m_pScene->GetMaterialUniformBufferIndex(),
        m_gShader.GetShaderProgram(),
        "gMaterial");

    return hs;
}

void GBuffer::drawGBuffer()
{
    m_gShader.useProgram();
    m_gFramebuffer.drawFramebuffer();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    size_t modelSize = m_pScene->GetModelSize();
    for (size_t i = 0; i < modelSize; ++i)
    {
        Model* pModel = m_pScene->GetModelPtr(i);
        Camera* pCam  = m_pScene->GetActivateCamera();

        glm::mat4 worldTransMatrix = pModel->m_pTrans->GetTransMatrix();
        glm::mat4 wvp = pCam->GetProjectionMatrix() * pCam->GetViewMatrix() * worldTransMatrix;

        GLint worldMatrixLocation = glGetUniformLocation(m_gShader.GetShaderProgram(), "worldMatrix");
        GLint wvpLocation         = glGetUniformLocation(m_gShader.GetShaderProgram(), "wvp");

        if (worldMatrixLocation >= 0 && wvpLocation >= 0)
        {
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransMatrix));
            glUniformMatrix4fv(wvpLocation, 1, GL_FALSE, glm::value_ptr(wvp));

            pModel->updateMaterial(m_pScene->GetUniformBufferMgr(),
                                   m_pScene->GetMaterialUniformBufferIndex());
            pModel->draw();
        }
        else
        {
            printf("Can't find gBuffer shader uniform block location!");
            assert(FALSE);
        }
    }

    m_gFramebuffer.finishDrawFramebuffer();
    m_gShader.finishProgram();
}

void GBuffer::readGBuffer(
    GLuint shaderProgram)
{
    m_gFramebuffer.getTexturePtr(GL_TEXTURE0)->
        bindTexture(GL_TEXTURE0, shaderProgram, "posTex", 0);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE1)->
        bindTexture(GL_TEXTURE1, shaderProgram, "normalTex", 1);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE2)->
        bindTexture(GL_TEXTURE2, shaderProgram, "texCoordTex", 2);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE3)->
        bindTexture(GL_TEXTURE3, shaderProgram, "kaTex", 3);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE4)->
        bindTexture(GL_TEXTURE4, shaderProgram, "kdTex", 4);
}
