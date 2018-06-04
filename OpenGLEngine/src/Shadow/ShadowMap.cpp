#include "ShadowMap.h"

#include "../Context/Scene.h"

ShadowMap::ShadowMap(
    Light*     pLight,         ///< The light be casted shadow
    const UINT depthTexWidth,
    const UINT depthTexHeight,
    const UINT samples)
    : m_pLight(pLight),
      m_shadowMapWidth(depthTexWidth),
      m_shadowMapHeight(depthTexWidth),
      m_shadowMapSamples(samples)
{ }

ShadowMap::~ShadowMap()
{ }

BOOL ShadowMap::initialize()
{

    float halfWidth = static_cast<float>(m_shadowMapWidth) * 0.0009f;
    float halfHeight = static_cast<float>(m_shadowMapHeight) * 0.0009f;

    m_pLightCamera = new OrthographicCamera(
        glm::vec3(), glm::vec3(), glm::vec3(0, 1, 0),
        0.0f, Rectangle(-halfWidth, halfWidth, -halfHeight, halfHeight), 0.1f, 1000.0f);

    m_shadowMapFramebuffer.createFramebuffer();
    m_shadowMapFramebuffer.createFramebufferTexture2D(GL_TEXTURE0,
                                                      GL_DEPTH_ATTACHMENT,
                                                      m_shadowMapWidth,
                                                      m_shadowMapHeight,
                                                      m_shadowMapSamples,
                                                      GL_DEPTH_COMPONENT,
                                                      GL_DEPTH_COMPONENT,
                                                      GL_FLOAT,
                                                      GL_CLAMP_TO_BORDER,
                                                      FALSE);

    GLfloat boarder[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    Texture2D* pDepthTexture = static_cast<Texture2D*>(m_shadowMapFramebuffer.getTexturePtr(GL_TEXTURE0));

    pDepthTexture->setBoarderColor(boarder);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer.getFramebufferHandle());

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_shadowMapShader.setShaderFiles("ShadowMap.vert", "ShadowMap.frag");

    BOOL hs = m_shadowMapShader.linkProgram();
    if (hs == TRUE)
    {
        initializeLightCamera();
    }
    else
    {
        m_shadowMapShader.assertErrors("Fail to compile shadow map shaders.\n");
    }

    return hs;
}

void ShadowMap::update(Light* pLight)
{
    m_pLight = pLight;

    DirectionalLight* pDirectionalLight = static_cast<DirectionalLight*>(m_pLight);
    Vector3 lightDir = pDirectionalLight->getDir();
    glm::vec3 glmLightDir = glm::vec3(lightDir.x, lightDir.y, lightDir.z);

    float lightPosScale = 179.0f;
    float halfWidth = static_cast<float>(m_shadowMapWidth) * 0.0009f;
    float halfHeight = static_cast<float>(m_shadowMapHeight) * 0.0009f;

    m_pLightCamera->setCamTrans(-glmLightDir * lightPosScale, glmLightDir, glm::vec3(0.0f, 1.0f, 0.0f));
}

void ShadowMap::drawShadowMap(Scene* pScene)
{
    assert(m_pLightCamera != NULL);

    m_shadowMapShader.useProgram();
    m_shadowMapFramebuffer.drawFramebuffer();

    glClear(GL_DEPTH_BUFFER_BIT);

    size_t modelSize = pScene->GetModelSize();
    for (size_t i = 0; i < modelSize; ++i)
    {
        Model* pModel = pScene->GetModelPtr(i);

        glm::mat4 worldMatrix    = pModel->m_pTrans->GetTransMatrix();
        glm::mat4 viewMatrix     = m_pLightCamera->GetViewMatrix();
        glm::mat4 prospectMatrix = m_pLightCamera->GetProjectionMatrix();
        glm::mat4 wvp            = prospectMatrix * viewMatrix * worldMatrix;

        GLint tranMatrixLocation = glGetUniformLocation(m_shadowMapShader.GetShaderProgram(), "wvp");

        if (tranMatrixLocation >= 0)
        {
            glUniformMatrix4fv(tranMatrixLocation, 1, GL_FALSE, glm::value_ptr(wvp));

            pModel->drawModelPos();
        }
        else
        {
            printf("Unable to get wvp matrix location in shadowMap shader");
            assert(FALSE);
        }
    }

    m_shadowMapFramebuffer.finishDrawFramebuffer();

    glDrawBuffer(GL_FRONT);
    glReadBuffer(GL_FRONT);

    m_shadowMapShader.finishProgram();
}

void ShadowMap::readShadowMap(
    const GLenum       texUnit,
    const GLuint       shaderProgram,
    const std::string& samplerName,
    const UINT         samplerIndex)
{
    m_shadowMapFramebuffer.getTexturePtr(GL_TEXTURE0)->
        bindTexture(texUnit, shaderProgram, samplerName, samplerIndex);
}

void ShadowMap::initializeLightCamera()
{
    switch (m_pLight->m_lightType)
    {
    case LightType::DIRECTIONAL_LIGHT:
    {
        update(m_pLight);

        break;
    }
    case LightType::POINT_LIGHT:
    {
        // TODO
        break;
    }
    case LightType::SPOT_LIGHT:
    {
        // TODO
        break;
    }
    default:
        printf("Unsupport light type.\n");
        assert(FALSE);
        break;
    }
}
