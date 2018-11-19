//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Cube.h"

Cube::Cube()
{
    m_verticesData     = &(m_cubeVertices[0]);
    m_verticesDataSize = sizeof(m_cubeVertices);

    m_indicesData      = &(m_cubeIndices[0]);
    m_indicesDataSize  = sizeof(m_cubeIndices);
    m_indicesDataCount = m_indicesDataSize / sizeof(m_cubeIndices[0]);
}

Cube::~Cube()
{
}
