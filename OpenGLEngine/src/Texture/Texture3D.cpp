#include "Texture.h"

Texture3D::Texture3D(const std::vector<std::string>& textureFile, 
                     GLenum                          format,
                     GLenum                          type,
                     GLenum                          wrapMethod,
                     BOOL                            mipmap)
    : Texture(TEXTURE_3D)
{
    // TODO
}

Texture3D::Texture3D(
        const UINT   texWidth,
        const UINT   texHeight,
        const UINT   layers,
        const UINT   samples,
        const GLenum loadFormat,
        const GLenum storeFormat,
        const GLenum type,
        const GLenum wrapMethod,
        const BOOL   mipmap,
        const void*  data)
    : Texture(TEXTURE_3D)
{
    m_textureWidth    = texWidth;
    m_textureHeight   = texHeight;
    m_textureLayer    = layers;
    m_samples         = samples;
    m_textureDataType = type;
    m_mipmap          = mipmap;

    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureHandle);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, storeFormat, m_textureWidth, m_textureHeight, layers, 0, loadFormat, type, 0);

    if (m_mipmap == TRUE)
    {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrapMethod);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrapMethod);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

Texture3D::~Texture3D()
{
    
}

void Texture3D::bindTexture(
    const GLenum       textureUnit,
    const GLuint       shaderProgram,
    const std::string& samplerName,
    const int          samplerIndex) 
{
    GLint textureLocation = glGetUniformLocation(shaderProgram, samplerName.data());

    assert(textureLocation >= 0);

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureHandle);
    glUniform1i(textureLocation, samplerIndex);
}

void Texture3D::unbindTexture()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}