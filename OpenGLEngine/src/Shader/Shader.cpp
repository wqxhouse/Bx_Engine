//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

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

