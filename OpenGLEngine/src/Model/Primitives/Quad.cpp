#include "Quad.h"

Quad::Quad()
{
    m_verticesData     = &(m_quadVertices[0]);
    m_verticesDataSize = sizeof(m_quadVertices);

    m_indicesData      = &(m_quadIndices[0]);
    m_indicesDataSize  = sizeof(m_quadIndices);
    m_indicesDataCount = m_indicesDataSize / sizeof(m_quadIndices[0]);
}

Quad::~Quad()
{
}
