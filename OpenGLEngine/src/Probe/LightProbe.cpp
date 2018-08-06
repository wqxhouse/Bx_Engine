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
    glm::vec3( 0.0f,  0.0f, -1.0f)
};

LightProbe::LightProbe(
    Scene*               pScene,
    const Math::Vector3& pos,
    const float          nearClip,
    const float          farClip)
    : m_pScene(pScene),
      probeResolution(128),
      m_pos(pos),
      m_nearClip(nearClip),
      m_farClip(farClip),
      firstDraw(TRUE)
{
}

LightProbe::~LightProbe()
{
    SafeDelete(m_pCubemap);
    for (UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        SafeDelete(m_pCubemapCam[i]);
    }
}

BOOL LightProbe::initialize()
{
    BOOL result = TRUE;

    m_pCubemap = new Cubemap(probeResolution);

    // TODO: Replace glm lib with Math lib
    for(UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    { 
        m_pCubemapCam[i] = 
            new ProspectiveCamera(glm::vec3(m_pos.x, m_pos.y, m_pos.z),
                                  glm::vec3(m_pos.x, m_pos.y, m_pos.z) + lightProbeFrontVector[i],
                                  glm::vec3(0.0f, 1.0f, 0.0f),
                                  0.0f,
                                  1.0f,
                                  m_nearClip,
                                  m_farClip,
                                  90.0f);
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
    // GLenum error = glGetError();

    Camera* pActiveCamera = m_pScene->GetActivateCamera();

    Math::Vector4& backColor = m_pScene->GetBackGroundColor();

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

        m_probeFbo.drawFramebuffer();

        glClearColor(backColor.r, backColor.g, backColor.b, backColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, probeResolution, probeResolution);

        m_pScene->draw(); // Render the light probe
        m_probeFbo.finishDrawFramebuffer();
    }

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
