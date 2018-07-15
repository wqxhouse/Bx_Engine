#pragma once

#include "../../Core/OpenGLPCH.h"

class Primitive
{
public:
    Primitive();
    ~Primitive();

    void initialize();

    virtual void draw() = 0;

protected:
    GLuint m_vertexArrayObj;
    GLuint m_vertexBufObj;
    GLuint m_indexBufObj;

    void*  m_verticesData;
    size_t m_verticesDataSize;

    void*  m_indicesData;
    size_t m_indicesDataSize;
};