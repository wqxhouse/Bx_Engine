#pragma once

#include "Shader.h"

class GraphicsShader : public Shader
{
public:
    GraphicsShader();
    ~GraphicsShader();

    void setShaderFiles(
        char* const vertexShaderFile,
        char* const fragmentShaderFile,
        char* const geometryShaderFile = NULL,  //Optional
        char* const tesShaderFile      = NULL,  //Optional
        char* const tcsShaderFile      = NULL); //Optional

    BOOL compileShaderProgram();

private:
    char* m_vertexShaderFile;
    char* m_fragmentShaderFile;
    char* m_geometryShaderFile;
    char* m_tesShaderFile;
    char* m_tcsShaderFile;
};