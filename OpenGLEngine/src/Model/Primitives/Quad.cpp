#include "Quad.h"

Quad::Quad()
{
}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_gQuadVertexBufObj);
    glDeleteBuffers(1, &m_gQuadIndexBufObj);
    glDeleteVertexArrays(1, &m_gQuadVAO);
}

void Quad::initialize()
{
    glGenVertexArrays(1, &m_gQuadVAO);
    glGenBuffers(1, &m_gQuadVertexBufObj);
    glGenBuffers(1, &m_gQuadIndexBufObj);

    glBindVertexArray(m_gQuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_gQuadVertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_gQuadVertices), m_gQuadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gQuadIndexBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_gQuadIndices), m_gQuadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Math::Vector3), 0);

    glBindVertexArray(0);
}

void Quad::draw()
{
    glBindVertexArray(m_gQuadVAO);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
