#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

GLuint Shader::useProgram()
{
    glUseProgram(m_shaderProgram);

    return m_shaderProgram;
}

