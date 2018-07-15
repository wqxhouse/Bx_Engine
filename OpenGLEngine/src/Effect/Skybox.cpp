#include "Skybox.h"

#include "../Context/Scene.h"

Skybox::Skybox(
    Scene*                          pScene,
    const std::vector<std::string>& skyboxImages)
    : m_pScene(pScene),
      m_skyboxCubemap(skyboxImages)
{
}

BOOL Skybox::initialize()
{
    BOOL result = TRUE;

    m_skyboxShader.setShaderFiles("Skybox.vert", "Skybox.frag");
    result = m_skyboxShader.linkProgram();

    assert(result == TRUE);

    m_skyboxCube.initialize();

    return result;
}

void Skybox::draw()
{
    glCullFace(GL_FRONT);

    GLuint shaderProgram = m_skyboxShader.useProgram();

    m_skyboxCubemap.bindTexture(GL_TEXTURE0, shaderProgram, "skyboxCubemap", 0);

    GLuint wvpLocation = glGetUniformLocation(shaderProgram, "wvp");

    if(wvpLocation >= 0)
    {
        Camera* pCam = m_pScene->GetActivateCamera();
        glm::mat4 wvp = pCam->GetProjectionMatrix() *
                        pCam->GetViewMatrix()       *
                        pCam->GetTrans().GetTransMatrix();

        glUniformMatrix4fv(wvpLocation, 1, GL_FALSE, glm::value_ptr(wvp));
    }

    m_skyboxCube.draw();

    m_skyboxCubemap.unbindTexture();

    m_skyboxShader.FinishProgram();

    glCullFace(GL_BACK);
}

Skybox::~Skybox()
{
}
