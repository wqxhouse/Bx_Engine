#include "Line.h"

Line::Line(const Math::Vector3& p1, const Math::Vector3& p2)
    : m_lineColor(1.0f, 1.0f, 1.0f)
{
    m_lineData[0] = p1;
    m_lineData[1] = p2;

    m_trans[0][0] = m_trans[1][1] = m_trans[2][2] = m_trans[3][3] = 1.0f;
}

Line::~Line()
{
    glDeleteBuffers(1, &m_vertexBufObj);
    glDeleteVertexArrays(1, &m_vertexArrayObj);
}

BOOL Line::initialize()
{
    m_lineShader.setShaderFiles("Line.vert", "Line.frag");
    BOOL result = m_lineShader.linkProgram();

    if (result == FALSE)
    {
        Shader::AssertErrors("Failed to compile line shaders.\n");
    }

    glGenVertexArrays(1, &m_vertexArrayObj);
    glGenBuffers(1, &m_vertexBufObj);

    glBindVertexArray(m_vertexArrayObj);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_lineData), m_lineData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);

    return TRUE;
}

void Line::draw()
{
    GLuint lineProgram = m_lineShader.useProgram();

    GLint transLocation = glGetUniformLocation(lineProgram, "transMat");
    GLint colorLocation = glGetUniformLocation(lineProgram, "color");

    if (transLocation >= 0 &&
        colorLocation >= 0)
    {
        glUniformMatrix4fv(transLocation, 1, GL_FALSE, &(m_trans[0][0]));
        glUniform3fv(colorLocation, 1, &(m_lineColor.x));
    }
    else
    {
        assert(false);
    }

    glBindVertexArray(m_vertexArrayObj);

    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);

    m_lineShader.FinishProgram();
}
