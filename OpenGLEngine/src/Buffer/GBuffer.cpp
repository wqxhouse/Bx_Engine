#include "GBuffer.h"

#include "../Context/Scene.h"

#define GBUFFER_TEXTURE_SIZE 6

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

    m_gFramebuffer.createFramebuffer();

    // Position buffer
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE0,
                                              GL_COLOR_ATTACHMENT0,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGB32F,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    // Normal buffer
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE1,
                                              GL_COLOR_ATTACHMENT1,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGB32F,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    // TexCoord0
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE2,
                                              GL_COLOR_ATTACHMENT2,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGB32F,
                                              GL_FLOAT,
                                              GL_REPEAT,
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
                                              GL_REPEAT,
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
                                              GL_REPEAT,
                                              FALSE);

    // Environment light
    m_gFramebuffer.createFramebufferTexture2D(GL_TEXTURE5,
                                              GL_COLOR_ATTACHMENT5,
                                              m_width,
                                              m_height,
                                              1,
                                              GL_RGBA,
                                              GL_RGB32F,
                                              GL_FLOAT,
                                              GL_REPEAT,
                                              FALSE);

    m_gShader.setShaderFiles("GBuffer.vert", "GBuffer.frag");
    result = m_gShader.linkProgram();

    if (result == FALSE)
    {
        printf("Fail to compile GBuffer shaders.\n");
        assert(FALSE);
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

    /*m_pScene->GetUniformBufferMgr()->bindUniformBuffer(
        m_pScene->GetShadowMap()->GetShadowResolutionUniformBufferIndex(),
        m_gShader.GetShaderProgram(),
        "shadowMapResolutionUniformBlock"
    );*/

    // G-Buffer Quad initialize
    glGenVertexArrays(1, &m_gQuadVAO);
    glGenBuffers(1, &m_gQuadVertexBufObj);
    glGenBuffers(1, &m_gQuadIndexBufObj);

    glBindVertexArray(m_gQuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_gQuadVertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_gQuadVertices), m_gQuadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gQuadIndexBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_gQuadIndices), m_gQuadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);

    glBindVertexArray(0);

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

        glm::mat4 worldTransMatrix = pModel->m_pTrans->GetTransMatrix();
        glm::mat4 wvp = pCam->GetProjectionMatrix() * pCam->GetViewMatrix() * worldTransMatrix;

        GLint worldMatrixLocation  = glGetUniformLocation(gShaderProgram, "worldMatrix");
        GLint lightTransLocation   = glGetUniformLocation(gShaderProgram, "lightTransWVP");
        GLint wvpLocation          = glGetUniformLocation(gShaderProgram, "wvp");
        GLint materialTypeLocation = glGetUniformLocation(gShaderProgram, "materialType");

        if (worldMatrixLocation  >= 0 &&
            lightTransLocation   >= 0 &&
            wvpLocation          >= 0 &&
            materialTypeLocation >= 0)
        {
            ShadowMap* pShadowMap = m_pScene->GetShadowMap();

            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransMatrix));

            glm::mat4 lightTransWVP = 
                pShadowMap->GetLightTransVP() * pModel->m_pTrans->GetTransMatrix();
            glUniformMatrix4fv(lightTransLocation, 1, GL_FALSE, glm::value_ptr(lightTransWVP));

            glUniformMatrix4fv(wvpLocation, 1, GL_FALSE, glm::value_ptr(wvp));

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

            pShadowMap->readShadowMap(GL_TEXTURE0, gShaderProgram, "shadowMapSampler", 0);

            pModel->draw();
        }
        else
        {
            printf("Can't find gBuffer shader uniform block location!");
            assert(FALSE);
        }
    }

    m_gFramebuffer.finishDrawFramebuffer();

    Shader::FinishProgram();
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
        bindTexture(GL_TEXTURE3, shaderProgram, "albedoTex", 3);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE4)->
        bindTexture(GL_TEXTURE4, shaderProgram, "specularTex", 4);

    m_gFramebuffer.getTexturePtr(GL_TEXTURE5)->
        bindTexture(GL_TEXTURE5, shaderProgram, "environmentLightTex", 5);
}

void GBuffer::draw()
{
    glBindVertexArray(m_gQuadVAO);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
