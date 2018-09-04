#include "GBuffer.h"

#include "../Context/Scene.h"

#define GBUFFER_TEXTURE_SIZE 8

GBuffer::GBuffer(
    Scene*     pScene,
    const UINT width,
    const UINT height)
    : m_gFramebuffer(GBUFFER_TEXTURE_SIZE),
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
    BOOL result = TRUE;

    // Position world buffer
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE0,
                                              GL_COLOR_ATTACHMENT0,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    Texture2D* pPosTex = static_cast<Texture2D*>(m_gFramebuffer.getTexturePtr(GL_TEXTURE0));
    pPosTex->setTextureSampleMethod(GL_NEAREST, GL_NEAREST);

    GLfloat posTexBoderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    pPosTex->setBoarderColor(posTexBoderColor);

    // Normal view buffer
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE1,
                                              GL_COLOR_ATTACHMENT1,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGB,
                                              GL_RGB32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    // TexCoord0
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE2,
                                              GL_COLOR_ATTACHMENT2,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RG,
                                              GL_RG32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    // Albedo material
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE3,
                                              GL_COLOR_ATTACHMENT3,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    // Specular material
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE4,
                                              GL_COLOR_ATTACHMENT4,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    // Environment light
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE5,
                                              GL_COLOR_ATTACHMENT5,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGB,
                                              GL_RGB32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    // Position view buffer
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE6,
                                              GL_COLOR_ATTACHMENT6,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGBA32F,
                                              GL_FLOAT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);

    /*m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE7,
                                              GL_COLOR_ATTACHMENT7,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RED_INTEGER,
                                              GL_R32I,
                                              GL_UNSIGNED_INT,
                                              GL_CLAMP_TO_BORDER,
                                              FALSE);*/

    m_gShader.setShaderFiles("GBuffer.vert", "GBuffer.frag");
    result = m_gShader.linkProgram();

    if (result == FALSE)
    {
        Shader::AssertErrors("Fail to compile GBuffer shaders.\n");
    }

    m_gFramebuffer.createRenderbufferAttachment(
        FRAMEBUFFER_DEPTH_RENDERBUFFER_ATTACHMENT, m_width, m_height);

    m_pScene->GetUniformBufferMgr()->bindUniformBuffer(
        m_pScene->GetMaterialUniformBufferIndex(),
        m_gShader.GetShaderProgram(),
        "gMaterial");

    m_pScene->GetUniformBufferMgr()->bindUniformBuffer(
        m_pScene->GetPBRMaterialUniformBufferIndex(),
        m_gShader.GetShaderProgram(),
        "gMaterialPBR");

    // G-Buffer Quad initialize
    m_gQuad.initialize();

    return result;
}

void GBuffer::drawGBuffer()
{
    GLuint gShaderProgram = m_gShader.useProgram();
    m_gFramebuffer.drawFramebuffer();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    size_t modelSize = m_pScene->GetModelSize();
    for (size_t i = 0; i < modelSize; ++i)
    {
        Model* pModel             = m_pScene->GetModelPtr(i);
        MaterialType materialType = pModel->GetModelMaterialType();

        Camera* pCam  = m_pScene->GetActivateCamera();

        // TODO: Combine all trans matrix into ubo
        glm::mat4 worldTransMatrix = pModel->m_pTrans->GetTransMatrix();
        glm::mat4 wv               = pCam->GetViewMatrix() * worldTransMatrix;
        glm::mat4 wvp              = pCam->GetProjectionMatrix() * wv;

        GLint cameraPosLocation    = glGetUniformLocation(gShaderProgram, "cameraPos");

        GLint worldLocation        = glGetUniformLocation(gShaderProgram, "worldMatrix");
        GLint wvLocation           = glGetUniformLocation(gShaderProgram, "wv");
        GLint wvpLocation          = glGetUniformLocation(gShaderProgram, "wvp");

        GLint materialTypeLocation = glGetUniformLocation(gShaderProgram, "materialType");

        if (cameraPosLocation    >= 0 &&
            worldLocation        >= 0 &&
            wvLocation           >= 0 &&
            wvpLocation          >= 0 &&
            materialTypeLocation >= 0)
        {
            glUniform3fv(cameraPosLocation, 1, glm::value_ptr(pCam->GetTrans().GetPos()));

            glUniformMatrix4fv(worldLocation, 1, GL_FALSE, glm::value_ptr(worldTransMatrix));
            glUniformMatrix4fv(wvLocation,    1, GL_FALSE, glm::value_ptr(wv));
            glUniformMatrix4fv(wvpLocation,   1, GL_FALSE, glm::value_ptr(wvp));

            if (useGlobalMaterial == FALSE)
            {
                glUniform1i(materialTypeLocation, materialType);

                switch (materialType)
                {
                case PHONG:
                    pModel->updateMaterial(m_pScene->GetUniformBufferMgr(),
                        m_pScene->GetMaterialUniformBufferIndex());
                    break;
                case ALBEDO:
                    assert(FALSE);
                    break;
                case COOKTORRANCE:
                    pModel->updateMaterial(m_pScene->GetUniformBufferMgr(),
                        m_pScene->GetPBRMaterialUniformBufferIndex());
                    break;
                default:
                    printf("Unsupported material type!\n");
                    assert(FALSE);
                    break;
                }
            }
            else
            {
                glUniform1i(materialTypeLocation, COOKTORRANCE);
                pModel->updateMaterial(m_pScene->GetUniformBufferMgr(),
                    m_pScene->GetPBRMaterialUniformBufferIndex());
            }

            pModel->draw();
        }
        else
        {
            printf("Can't find gBuffer shader uniform block location!\n");
            assert(FALSE);
        }
    }

    m_gFramebuffer.finishDrawFramebuffer();

    Shader::FinishProgram();
}

void GBuffer::readGBuffer(
    const GLuint shaderProgram)
{
    m_gFramebuffer.getTexturePtr(GL_TEXTURE0)->
        bindTexture(GL_TEXTURE0, shaderProgram, "posTex", 0);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE1)->
        bindTexture(GL_TEXTURE1, shaderProgram, "normalTex", 1);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE2)->
        bindTexture(GL_TEXTURE2, shaderProgram, "texCoordTex", 2);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE3)->
        bindTexture(GL_TEXTURE3, shaderProgram, "albedoTex", 3);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE4)->
        bindTexture(GL_TEXTURE4, shaderProgram, "specularTex", 4);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE5)->
        bindTexture(GL_TEXTURE5, shaderProgram, "environmentLightTex", 5);
}

void GBuffer::readGBuffer(
    const GLuint       shaderProgram,
    const std::string& textureName,
    const GLenum       texUnit)
{
    assert(texUnit >= GL_TEXTURE0);

    m_gFramebuffer.getTexturePtr(texUnit)->
        bindTexture(texUnit, shaderProgram, textureName, texUnit - GL_TEXTURE0);
}

void GBuffer::draw()
{
    m_gQuad.draw();
}
