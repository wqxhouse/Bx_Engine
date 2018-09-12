#include "Quad.h"

Quad::Quad(
    const Math::Vector2& pos,
    const float          size)
    : Primitive(BxsPrimitiveTriangle)
{
    for (size_t i = 0; i < 4; ++i)
    {
        m_quadVertices[i] = m_quadVertices[i] * size + Math::Vector3(pos.x, pos.y, 0.0f);
    }

    m_verticesData     = &(m_quadVertices[0]);
    m_verticesDataSize = sizeof(m_quadVertices);

    m_indicesData      = &(m_quadIndices[0]);
    m_indicesDataSize  = sizeof(m_quadIndices);
    m_indicesDataCount = m_indicesDataSize / sizeof(m_quadIndices[0]);
}

Quad::~Quad()
{
}
