#pragma once

#include "../../Core/OpenGLPCH.h"

class Primitive
{
public:
    Primitive(const BxsPrimitiveType primitiveType);
    ~Primitive();

    void initialize();

    void draw();

protected:
    GLuint m_vertexArrayObj;
    GLuint m_vertexBufObj;
    GLuint m_indexBufObj;

    void*  m_verticesData;
    size_t m_verticesDataSize;

    void*  m_indicesData;
    size_t m_indicesDataSize;
    UINT   m_indicesDataCount;

    GLenum m_primitiveType;
    UINT   m_primitiveCount;
};