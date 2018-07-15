#include "Cube.h"

Cube::Cube()
{
    m_verticesData     = &(m_cubeVertices[0]);
    m_verticesDataSize = sizeof(m_cubeVertices);

    m_indicesData      = &(m_cubeIndices[0]);
    m_indicesDataSize  = sizeof(m_cubeIndices);
}

Cube::~Cube()
{
}
