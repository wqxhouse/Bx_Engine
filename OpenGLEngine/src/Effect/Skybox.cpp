#include "Skybox.h"

Skybox::Skybox(
    const std::vector<std::string>& skyboxImages)
    : m_skyboxCubemap(skyboxImages)
{
}

BOOL Skybox::initialize()
{
    BOOL result = TRUE;

    m_skyboxShader.setShaderFiles("Skybox.vert", "Skybox.frag");
    result = m_skyboxShader.linkProgram();

    assert(result == TRUE);

    return result;
}

void Skybox::draw()
{
    GLuint shaderProgram = m_skyboxShader.useProgram();

    m_skyboxCubemap.bindTexture(GL_TEXTURE0, shaderProgram, "skyboxCubemap", 0);

    // TODO: Draw

    m_skyboxCubemap.unbindTexture();

    m_skyboxShader.FinishProgram();
}

Skybox::~Skybox()
{
}
