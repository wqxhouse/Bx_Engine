#include "Primitive.h"

#include "../../Math/Vector3.h"

Primitive::Primitive(const BxsPrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case BxsPrimitivePoint:
            m_primitiveType  = GL_POINT;
            m_primitiveCount = 1;
            break;
        case BxsPrimitiveLine:
            m_primitiveType  = GL_LINE;
            m_primitiveCount = 2;
            break;
        case BxsPrimitiveTriangle:
            m_primitiveType  = GL_FILL;
            m_primitiveCount = 3;
            break;
        default:
            assert(false);
            break;
    }
}

Primitive::~Primitive()
{
    glDeleteBuffers(1, &m_vertexBufObj);
    glDeleteBuffers(1, &m_indexBufObj);
    glDeleteVertexArrays(1, &m_vertexArrayObj);
}

void Primitive::initialize()
{
    glGenVertexArrays(1, &m_vertexArrayObj);
    glGenBuffers(1, &m_vertexBufObj);
    glGenBuffers(1, &m_indexBufObj);

    glBindVertexArray(m_vertexArrayObj);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, m_verticesDataSize, m_verticesData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesDataSize, m_indicesData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, m_primitiveCount, GL_FLOAT, GL_FALSE, sizeof(Math::Vector3), 0);

    glBindVertexArray(0);
}

void Primitive::draw()
{
    glBindVertexArray(m_vertexArrayObj);
    glEnableVertexAttribArray(0);
    glDrawElements(m_primitiveType, m_indicesDataCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
