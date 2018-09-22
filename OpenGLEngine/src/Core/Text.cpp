#include "Text.h"

#include "../Context/Scene.h"

Text::Text()
    : m_fontTexture("../resources/textures/font/TimesNewRoman_Font.bmp")
{ }

Text::~Text()
{
    glDeleteBuffers(1, &m_vertexBufObj);
    glDeleteBuffers(1, &m_indexBufObj);
    glDeleteVertexArrays(1, &m_vertexArrayObj);
}

void Text::initialize()
{
    m_textShader.setShaderFiles("Text.vert", "Text.frag");
    BOOL status = m_textShader.linkProgram();

    if (status == FALSE)
    {
        Shader::AssertErrors("Fail to compile text shaders.");
    }

    glGenVertexArrays(1, &m_vertexArrayObj);
    glGenBuffers(1, &m_vertexBufObj);
    glGenBuffers(1, &m_indexBufObj);

    glBindVertexArray(m_vertexArrayObj);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexUV), m_vertexUV, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUV), (GLvoid*)(offsetof(VertexUV, VertexUV::pos)));

    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUV), (GLvoid*)(offsetof(VertexUV, VertexUV::uv)));

    glBindVertexArray(0);
}

void Text::RenderText(
    const Scene*         pScene,
    const std::string&   text,
    const Math::Vector2& pos,
    const float          textSize)
{
    Setting* pSetting = pScene->GetSetting();

    size_t length = text.length();

    float row    = 0.0f;
    float column = 0.0f;

    for (size_t i = 0; i < length; ++i)
    {
        const char c = text[i];

        if (c == '\n') 
        {
            row   += 1.0f;
            column = 0.0f;
        }
        else
        {
            assert(c >= ' ' && c <= '_');

            float localTextSizeX = textSize / static_cast<float>(pSetting->resolution.width);
            float localTextSizeY = textSize / static_cast<float>(pSetting->resolution.height);

            float x = pos.x / static_cast<float>(pSetting->resolution.width);
            x += column * localTextSizeX;
            column += 1.0f;

            float y = pos.y / static_cast<float>(pSetting->resolution.height);
            y -= row * localTextSizeY;

            float u = (c % 8) * fontTexSize;
            float v = ((c - 32) >> 3) * fontTexSize;

            // Shrink the sampling area for avoiding useless region on font texture
            float halfFontTexSize = fontTexSize * 0.5f;

            m_vertexUV[0] = { {x, y}, {u, v + halfFontTexSize } };
            m_vertexUV[1] = { {x + localTextSizeX, y}, {u + halfFontTexSize, v + halfFontTexSize } };
            m_vertexUV[2] = { {x, y + localTextSizeY}, {u, v } };
            m_vertexUV[3] = { {x + localTextSizeX, y + localTextSizeY}, {u + halfFontTexSize, v } };

            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufObj);
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexUV), m_vertexUV, GL_DYNAMIC_DRAW);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            GLuint textShaderProgram = m_textShader.useProgram();

            m_fontTexture.bindTexture(GL_TEXTURE0, textShaderProgram, "fontTex");

            glBindVertexArray(m_vertexArrayObj);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            m_fontTexture.unbindTexture();

            m_textShader.FinishProgram();

            glDisable(GL_BLEND);
        }
    }
}
