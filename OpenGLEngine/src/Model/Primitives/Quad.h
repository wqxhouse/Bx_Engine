#pragma once

#include "../../Core/OpenGLPCH.h"
#include "../../Math/Vector3.h"

class Quad
{
public:
    Quad();
    ~Quad();

    void initialize();
    void draw();

private:
    GLuint   m_gQuadVAO;
    GLuint   m_gQuadVertexBufObj;
    GLuint   m_gQuadIndexBufObj;

    Math::Vector3 m_gQuadVertices[4] =
    {
        { -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
        {  1.0f, 1.0f, 1.0f }
    };

    UINT m_gQuadIndices[6] =
    {
        0, 1, 2,
        1, 3, 2
    };
};