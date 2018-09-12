#include "Cube.h"

Cube::Cube()
    : Primitive(BxsPrimitiveTriangle)
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
