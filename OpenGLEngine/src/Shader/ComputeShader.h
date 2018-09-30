#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader();
    ~ComputeShader();

    BOOL compileShaderProgram();

    inline void setShader(char* const computeShaderFile) { m_computeShaderFile = computeShaderFile; }

private:
    char* m_computeShaderFile;
};