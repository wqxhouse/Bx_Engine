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
    result = m_skyboxShader.compileShaderProgram();

    assert(result == TRUE);

    m_skyboxCube.initialize();

    return result;
}

void Skybox::draw()
{
    glCullFace(GL_FRONT);

    GLuint shaderProgram = m_skyboxShader.useProgram();

    m_skyboxCubemap.bindTexture(GL_TEXTURE0, shaderProgram, "skyboxCubemap");

    GLuint wvpLocation = glGetUniformLocation(shaderProgram, "wvp");

    if(wvpLocation >= 0)
    {
        Camera* pCam = m_pScene->GetActivateCamera();
        glm::mat4 wvp = ToGLMMat4(pCam->GetProjectionMatrix())      *
                        ToGLMMat4(pCam->GetViewMatrix()) *
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
