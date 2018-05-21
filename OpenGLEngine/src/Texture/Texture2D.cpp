#include "Texture.h"

//Image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D::Texture2D(
    const UINT   texWidth,
    const UINT   texHeight,
    const GLenum format,
    const GLenum type,
    const GLenum wrapMethod,
    const BOOL   mipmap)
    : Texture(TEXTURE_2D)
{
    m_textureWidth  = texWidth;
    m_textureHeight = texHeight;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureWidth, m_textureHeight,
                 0, GL_RGBA, type, 0);

    if (mipmap == TRUE)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMethod);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMethod);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(
    const std::string& textureFile,
    const GLenum       format,
    const GLenum       type,
    const GLenum       wrapMethod,
    const BOOL         mipmap)
    : Texture(TEXTURE_2D)
{
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    m_textureData = stbi_load(textureFile.data(),
                              reinterpret_cast<int*>(&m_textureWidth),
                              reinterpret_cast<int*>(&m_textureHeight),
                              reinterpret_cast<int*>(&m_textureType),
                              STBI_rgb_alpha);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureWidth, m_textureHeight,
                 0, GL_RGBA, type, m_textureData);

    if (mipmap == TRUE)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMethod);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMethod);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::setBoarderColor(GLfloat borderColor[4])
{
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bindTexture(const GLenum textureIndex,
                            const GLuint shaderProgram,
                            const std::string & samplerName,
                            const int samplerIndex)
{
    glActiveTexture(textureIndex);
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    glUniform1i(glGetUniformLocation(shaderProgram, samplerName.data()), samplerIndex);
}

void Texture2D::unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void * Texture2D::getTextureData()
{
    return m_textureData;
}

Texture2D::~Texture2D()
{
    stbi_image_free(m_textureData);
}
