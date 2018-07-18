#pragma once

#include "ShaderCompiler.h"

class Shader
{
public:
    Shader();

    ~Shader();

    void setShaderFiles(
        char* const vertexShaderFile,
        char* const fragmentShaderFile,
        char* const geometryShaderFile = NULL,  //Optional
        char* const tesShaderFile      = NULL,  //Optional
        char* const tcsShaderFile      = NULL); //Optional

    BOOL linkProgram();

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

private:
    GLuint m_shaderProgram;

    ShaderCompiler m_shaderCompiler;

    char* m_vertexShaderFile;
    char* m_fragmentShaderFile;
    char* m_geometryShaderFile;
    char* m_tesShaderFile;
    char* m_tcsShaderFile;
};