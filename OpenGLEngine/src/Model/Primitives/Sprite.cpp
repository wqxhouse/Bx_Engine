#include "Sprite.h"

#include "../../Context/Scene.h"

Sprite::Sprite(
        Scene*               pScene,
        const Math::Vector3& spritePosition,
        const RectangleSize& spriteSize)
    : m_spritePosition(spritePosition),
      m_spriteSize(spriteSize)
{
    Resolution resolution = pScene->GetSetting()->resolution;

    float x = spritePosition.x / static_cast<float>(resolution.width);
    float y = spritePosition.y / static_cast<float>(resolution.height);

    float spriteSizeX = spriteSize.width  / static_cast<float>(resolution.width);
    float spriteSizeY = spriteSize.height / static_cast<float>(resolution.height);

    m_spriteVertices[0] = { { x, y, 0.0f }, { 0.0f, 0.0f } };
    m_spriteVertices[1] = { { x + spriteSizeX, y, 0.0f }, { 1.0f, 0.0f } };
    m_spriteVertices[2] = { { x, y + spriteSizeY, 0.0f }, { 0.0f, 1.0f } };
    m_spriteVertices[3] = { { x + spriteSizeX, y + spriteSizeY, 0.0f }, { 1.0f, 1.0f } };
}

Sprite::~Sprite()
{
    glDeleteBuffers(1, &m_vertexBufObj);
    glDeleteBuffers(1, &m_indexBufObj);
    glDeleteVertexArrays(1, &m_vertexArrayObj);
}

BOOL Sprite::initialize()
{
    BOOL status = TRUE;

    m_spriteShader.setShaderFiles("Sprite.vert", "Sprite.frag");
    status = m_spriteShader.linkProgram();
    if (status != TRUE)
    {
        Shader::AssertErrors("Failed to compile sprite shader!\n");
    }

    glGenVertexArrays(1, &m_vertexArrayObj);
    glGenBuffers(1, &m_vertexBufObj);
    glGenBuffers(1, &m_indexBufObj);

    glBindVertexArray(m_vertexArrayObj);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_spriteVertices), m_spriteVertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_spriteIndices), m_spriteIndices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexUV), (GLvoid*)(offsetof(VertexUV, VertexUV::pos)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUV), (GLvoid*)(offsetof(VertexUV, VertexUV::uv)));

    glBindVertexArray(0);

    return status;
}

void Sprite::draw()
{
    m_spriteShader.useProgram();

    assert(m_pSpriteTexture != NULL);

    if (m_pSpriteTexture != NULL)
    {
        m_pSpriteTexture->bindTexture(GL_TEXTURE0, m_spriteShader.GetShaderProgram(), "spriteTex");
    }

    glBindVertexArray(m_vertexArrayObj);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_spriteShader.FinishProgram();
}

void Sprite::BindTexture(
    Texture2D* pSpriteTexture)
{
    m_pSpriteTexture = pSpriteTexture;
}
