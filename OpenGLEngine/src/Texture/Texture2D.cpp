#include "Texture.h"

//Image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D::Texture2D(
    const UINT   texWidth,
    const UINT   texHeight,
    const UINT   samples,
    const GLenum format,
    const GLenum type,
    const GLenum wrapMethod,
    const BOOL   mipmap)
    : Texture(TEXTURE_2D), m_samples(samples)
{
    m_textureWidth  = texWidth;
    m_textureHeight = texHeight;

    GLenum glTextureType = ((m_samples < 2) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE);

    glBindTexture(glTextureType, m_textureHandle);
    glTexImage2D(glTextureType, 0, format, m_textureWidth, m_textureHeight,
        0, format, type, 0);

    if (mipmap == TRUE)
    {
        glTexParameteri(glTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(glTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(glTextureType);
    }
    else
    {
        glTexParameteri(glTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(glTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glTexParameteri(glTextureType, GL_TEXTURE_WRAP_S, wrapMethod);
    glTexParameteri(glTextureType, GL_TEXTURE_WRAP_T, wrapMethod);

    glBindTexture(glTextureType, 0);
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

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_textureWidth, m_textureHeight,
                 0, format, type, m_textureData);

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
    GLenum glTextureType = ((m_samples < 2) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE);

    glBindTexture(glTextureType, m_textureHandle);
    glTexParameterfv(glTextureType, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(glTextureType, 0);
}

void Texture2D::bindTexture(const GLenum textureUnit,
                            const GLuint shaderProgram,
                            const std::string& samplerName,
                            const int samplerIndex)
{
    GLint textureLocation = glGetUniformLocation(shaderProgram, samplerName.data());

    if (textureLocation >= 0)
    {
        GLenum glTextureType = ((m_samples < 2) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE);

        glActiveTexture(textureUnit);
        glBindTexture(glTextureType, m_textureHandle);
        glUniform1i(textureLocation, samplerIndex);
    }
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
