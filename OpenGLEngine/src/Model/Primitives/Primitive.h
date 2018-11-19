//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../../Core/OpenGLPCH.h"

class Primitive
{
public:
    Primitive();
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
};