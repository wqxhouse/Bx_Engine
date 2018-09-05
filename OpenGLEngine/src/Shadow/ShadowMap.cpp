#include "ShadowMap.h"

#include "../Context/Scene.h"

ShadowMap::ShadowMap(
    Scene*     pScene,  ///< Pointer to the scene
    Light*     pLight)  ///< The light be casted shadow
    : m_pScene(pScene),
      m_pLight(pLight)
{
    Setting* pSetting = pScene->GetSetting();

    m_shadowResolution = pSetting->m_graphicsSetting.shadowResolution;
    m_shadowMapSamples = 1;

    // TODO: Multi-sampled shadow map
    // m_shadowMapSamples = pSetting->m_graphicsSetting.antialasing;
}

ShadowMap::~ShadowMap()
{ }

BOOL ShadowMap::initialize()
{
    float halfWidth  = static_cast<float>(m_shadowResolution.width)  * 0.005f;
    float halfHeight = static_cast<float>(m_shadowResolution.height) * 0.005f;

    float offset = 0.0f;

    switch (m_pLight->GetLightType())
    {
        case LightType::DIRECTIONAL_LIGHT:
        {
            m_pLightCamera = new OrthographicCamera(
                glm::vec3(), glm::vec3(), glm::vec3(0, 1, 0),
                0.0f, Rectangle(-halfWidth  + offset,
                                 halfWidth  + offset,
                                -halfHeight + offset,
                                 halfHeight + offset), 0.1f, 10000.0f);
            break;
        }
        case LightType::POINT_LIGHT:
        {
            break;
        }
        case LightType::SPOT_LIGHT:
        {
            SpotLight* pSpotLight = static_cast<SpotLight*>(m_pLight);

            Math::Vector3 pos = pSpotLight->GetPos();
            Math::Vector3 dir = pSpotLight->GetDir();
            Math::Vector3 center = pos + dir;

            Math::Vector3 up(0.0f, 1.0f, 0.0f);
            if (dir.y > 0.99f || dir.y < -0.99f)
            {
                up = Math::Vector3(0.0f, 0.0f, 1.0f);
            }

            m_pLightCamera = new ProspectiveCamera(
                glm::vec3(pos.x, pos.y, pos.z),
                glm::vec3(center.x, center.y, center.z),
                glm::vec3(up.x, up.y, up.z),
                0.0f,
                static_cast<float>(m_shadowResolution.width) / static_cast<float>(m_shadowResolution.height),
                0.1f,
                1000.0f,
                pSpotLight->GetOuterAngleDegree());

            break;
        }
        default:
            printf("Unknown light type!\n");
            assert(false);
            break;
    }

    /*m_shadowMapFramebuffer.createFramebufferTexture2D(GL_TEXTURE0,
                                                      GL_DEPTH_ATTACHMENT,
                                                      m_shadowResolution.width,
                                                      m_shadowResolution.height,
                                                      m_shadowMapSamples,
                                                      GL_DEPTH_COMPONENT,
                                                      GL_DEPTH_COMPONENT,
                                                      GL_FLOAT,
                                                      GL_CLAMP_TO_EDGE,
                                                      FALSE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer.getFramebufferHandle());

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    m_shadowMapShader.setShaderFiles("ShadowMap.vert", "ShadowMap.frag");

    BOOL hs = m_shadowMapShader.linkProgram();
    if (hs == TRUE)
    {
        initializeLightCamera();
    }
    else
    {
        Shader::AssertErrors("Fail to compile shadow map shaders.\n");
    }

    // Bind shadow resolution with shadow map
    UniformBufferMgr* pUniformBufMgr = m_pScene->GetUniformBufferMgr();

    m_shadowResolutionUniformBufferIndex =
        pUniformBufMgr->createUniformBuffer(GL_STATIC_DRAW,
                                            sizeof(Resolution),
                                            &m_shadowResolution);

    return hs;
}

void ShadowMap::update(Light* pLight)
{
    m_pLight = pLight;

    Vector3 lightDir;

    switch (pLight->GetLightType())
    {
        case LightType::DIRECTIONAL_LIGHT:
        {
            DirectionalLight* pDirectionalLight = static_cast<DirectionalLight*>(m_pLight);
            lightDir = pDirectionalLight->GetDir();
            break;
        }
        case LightType::POINT_LIGHT: 
            break;
        case LightType::SPOT_LIGHT:
        {
            SpotLight* pSpotLight = static_cast<SpotLight*>(m_pLight);
            lightDir = pSpotLight->GetDir();
            break;
        }
        default:
            printf("Unknown light type!\n");
            assert(false);
            break;
    }

    glm::vec3 glmLightDir = glm::vec3(lightDir.x, lightDir.y, lightDir.z);

    float lightPosScale = 5.0f;
    m_pLightCamera->setCamTrans(-glmLightDir * lightPosScale, glmLightDir, m_pLightCamera->GetTrans().GetUp());
}

void ShadowMap::drawShadowMap(Scene* pScene)
{
    assert(m_pLightCamera != NULL);

    m_shadowMapShader.useProgram();
    //m_shadowMapFramebuffer.drawFramebuffer();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_shadowResolution.width, m_shadowResolution.height);

    size_t modelSize = pScene->GetModelSize();

    glm::mat4 viewMatrix     = m_pLightCamera->GetViewMatrix();
    glm::mat4 prospectMatrix = m_pLightCamera->GetProjectionMatrix();

    for (size_t i = 0; i < modelSize; ++i)
    {
        Model* pModel = pScene->GetModelPtr(i);

        glm::mat4 worldMatrix    = pModel->m_pTrans->GetTransMatrix();
        glm::mat4 wvp            = prospectMatrix * viewMatrix * worldMatrix;

        GLint transMatrixLocation = glGetUniformLocation(m_shadowMapShader.GetShaderProgram(), "wvp");

        if (transMatrixLocation >= 0)
        {
            glUniformMatrix4fv(transMatrixLocation, 1, GL_FALSE, glm::value_ptr(wvp));

            pModel->drawModelPos();
        }
        else
        {
            printf("Unable to get wvp matrix location in shadowMap shader\n");
            assert(FALSE);
        }
    }

    //m_shadowMapFramebuffer.finishDrawFramebuffer();

    Shader::FinishProgram();
}

/*void ShadowMap::readShadowMap(
    const GLenum       texUnit,
    const GLuint       shaderProgram,
    const std::string& samplerName,
    const UINT         samplerIndex)
{
    m_shadowMapFramebuffer.getTexturePtr(GL_TEXTURE0)->
        bindTexture(texUnit, shaderProgram, samplerName, samplerIndex);
}*/

void ShadowMap::initializeLightCamera()
{
    switch (m_pLight->GetLightType())
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
