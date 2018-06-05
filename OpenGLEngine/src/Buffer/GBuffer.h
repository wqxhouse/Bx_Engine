#pragma once

#include "Framebuffer.h"
#include "../Shader/Shader.h"
#include "../Math/Vector3.h"

class Scene;

class GBuffer
{
public:
    GBuffer(
        Scene*       pScene,
        const UINT   width,
        const UINT   height);
    ~GBuffer();

    BOOL initialize();
    void drawGBuffer();
    void readGBuffer(GLuint shaderProgram);

    void draw();

private:
    Scene*      m_pScene; // Pointer to the scene

    Framebuffer m_gFramebuffer;
    Shader      m_gShader;

    UINT m_width;
    UINT m_height;

    GLuint   m_gQuadVAO;
    GLuint   m_gQuadVertexBufObj;
    GLuint   m_gQuadIndexBufObj;

    Math::Vector3 m_gQuadVertices[4] =
    {
        { -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f, 0.0f },
        {  1.0f,  1.0f, 1.0f }
    };

    UINT m_gQuadIndices[6] =
    {
        0, 1, 2,
        1, 3, 2
    };
};