#pragma once

#include "ShaderCompiler.h"

class Shader
{
public:
    Shader();

    virtual ~Shader();

    virtual BOOL compileShaderProgram() { return TRUE; }

    GLuint useProgram();

    static inline void FinishProgram()
    {
        glUseProgram(0);
    }

    static inline void AssertErrors(const char* const str = "Fail to compile shaders.\n")
    {
        printf(str);
        assert(FALSE);
    }

    inline GLuint GetShaderProgram() const { return m_shaderProgram; }

protected:
    GLuint         m_shaderProgram;
    ShaderCompiler m_shaderCompiler;
};