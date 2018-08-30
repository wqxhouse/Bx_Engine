#include "LightProbe.h"
#include "../Context/Scene.h"

static GLenum lightProbeFaces[CUBE_MAP_FACE_NUM] =
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

glm::vec3 lightProbeFrontVector[6] =
{
    glm::vec3( 1.0f,  0.0f,  0.0f),
    glm::vec3(-1.0f,  0.0f,  0.0f),
    glm::vec3( 0.0f,  1.0f,  0.0f),
    glm::vec3( 0.0f, -1.0f,  0.0f),
    glm::vec3( 0.0f,  0.0f,  1.0f),
    glm::vec3( 0.0f,  0.0f, -1.0f),
};

LightProbe::LightProbe(
    Scene*               pScene,
    const Math::Vector3& pos,
    const float          nearClip,
    const float          farClip)
    : m_pScene(pScene),
      m_probeResolution(128),
      m_pos(pos),
      m_nearClip(nearClip),
      m_farClip(farClip),
      firstDraw(TRUE)
{
}

LightProbe::~LightProbe()
{
    for (UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        SafeDelete(m_pCubemapCam[i]);
    }
}

BOOL LightProbe::initialize()
{
    BOOL result = TRUE;

    m_pCubemap = new Cubemap(m_probeResolution);

    // TODO: Replace glm lib with Math lib
    for(UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    { 
        if (i != 2 && i != 3)
        { 
            m_pCubemapCam[i] =
                new ProspectiveCamera(glm::vec3(m_pos.x, m_pos.y, m_pos.z),
                                      glm::vec3(m_pos.x, m_pos.y, m_pos.z) + lightProbeFrontVector[i],
                    // TODO: Figure out the reason of why we need to flip the camera (up to negative y)
                                      glm::vec3(0.0f, -1.0f, 0.0f),
                                      0.0f,
                                      1.0f,
                                      m_nearClip,
                                      m_farClip,
                                      90.0f);
        }
        else
        {
            m_pCubemapCam[i] = 
                new ProspectiveCamera(glm::vec3(m_pos.x, m_pos.y, m_pos.z),
                                      glm::vec3(m_pos.x, m_pos.y, m_pos.z) + lightProbeFrontVector[i],
                                      glm::vec3(0.0f, 0.0f, 1.0f),
                                      0.0f,
                                      1.0f,
                                      m_nearClip,
                                      m_farClip,
                                      90.0f);
        }
    }

    return result;
}

void LightProbe::update(
    const Math::Vector3& pos)
{
    for (UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        // TODO: Replace glm with Math lib
        m_pCubemapCam[i]->setCamTrans(
            glm::vec3(pos.x, pos.y, pos.z),
            glm::vec3(pos.x, pos.y, pos.z) + lightProbeFrontVector[i],
            glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void LightProbe::draw()
{
    Camera* pActiveCamera = m_pScene->GetActivateCamera();

    for (UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        m_pScene->SetActiveCamera(m_pCubemapCam[i]);

        if (i == 0)
        {
            if (firstDraw == TRUE)
            {
                m_probeFbo.attachCubemap(
                    GL_TEXTURE0, GL_COLOR_ATTACHMENT0, m_pCubemap, lightProbeFaces[i], 1, TRUE);

                firstDraw = FALSE;
            }
            else
            {
                m_probeFbo.attachCubemap(
                    GL_TEXTURE0, GL_COLOR_ATTACHMENT0, m_pCubemap, lightProbeFaces[i], 1, FALSE);
            }
        }
        else
        {
            m_probeFbo.attachCubemap(
                GL_TEXTURE0, GL_COLOR_ATTACHMENT0, m_pCubemap, lightProbeFaces[i], 1, FALSE);
        }
        // Render the light probe
        if (m_pScene->GetSetting()->m_graphicsSetting.renderingMethod == RenderingMethod::FORWARD_RENDERING)
        {
            m_probeFbo.drawFramebuffer();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, m_probeResolution, m_probeResolution);

            m_pScene->draw();

            m_probeFbo.finishDrawFramebuffer();
        }
        else
        {
            glViewport(0, 0, m_pScene->GetSetting()->resolution.width, m_pScene->GetSetting()->resolution.height);

            m_pScene->m_pGBuffer->drawGBuffer();

            /// NOTE: No need to enable SSAO for light probe

            m_probeFbo.drawFramebuffer();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, m_probeResolution, m_probeResolution);

            if (m_pScene->m_skyboxImages.size() == 6)
            {
                m_pScene->m_pSkybox->draw();
            }

            GLuint gShaderProgram = m_pScene->m_deferredRenderingShader.useProgram();
            m_pScene->m_pGBuffer->readGBuffer(gShaderProgram);

            GLint eyeLocation     = glGetUniformLocation(gShaderProgram, "eyePos");
            GLint viewMatLocation = glGetUniformLocation(gShaderProgram, "viewMat");
            GLint useSsaoLocation = glGetUniformLocation(gShaderProgram, "useSsao");

            if (eyeLocation     >= 0 &&
                viewMatLocation >= 0 &&
                useSsaoLocation >= 0)
            {
                Camera* pCam = m_pScene->GetActivateCamera();

                glUniform3fv(eyeLocation, 1, glm::value_ptr(pCam->GetTrans().GetPos()));
                glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, glm::value_ptr(pCam->GetViewMatrix()));

                // Disable SSAO
                glUniform1i(useSsaoLocation, 0);

                // Store the original render resolution
                Resolution resolution            = m_pScene->m_pSetting->resolution;
                m_pScene->m_pSetting->resolution = { m_probeResolution, m_probeResolution };

                m_pScene->m_uniformBufferMgr.updateUniformBufferData(
                    m_pScene->m_resolutionUniformBufferIndex,
                    sizeof(Resolution),
                    &(m_pScene->m_pSetting->resolution));

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                m_pScene->m_pGBuffer->draw();

                m_probeFbo.finishDrawFramebuffer();

                glDisable(GL_BLEND);

                // Retrive the resolution data back
                m_pScene->m_pSetting->resolution = resolution;

                m_pScene->m_uniformBufferMgr.updateUniformBufferData(
                    m_pScene->m_resolutionUniformBufferIndex,
                    sizeof(Resolution),
                    &(m_pScene->m_pSetting->resolution));
            }
        }
    }

    // Generating mipmaps for light probe
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_pCubemap->GetTextureHandle());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glViewport(0, 0, m_pScene->GetSetting()->resolution.width, m_pScene->GetSetting()->resolution.height);

    // Return the activate camera back
    m_pScene->SetActiveCamera(pActiveCamera);
}

void LightProbe::readLightProbe(
    const GLuint       shaderProgram,
    const std::string& textureName,
    const GLenum       texUnit)
{
    m_probeFbo.getTexturePtr(GL_TEXTURE0)
        ->bindTexture(texUnit, shaderProgram, textureName, texUnit - GL_TEXTURE0);
}
