#include "Texture.h"

#include "stb_image.h"

#define CUBE_MAP_FACE_NUM 6

static GLenum cubeFace[CUBE_MAP_FACE_NUM] =
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

Cubemap::Cubemap(
    const std::vector<std::string>& textureFile,
    const GLenum                    format,    
    const GLenum                    type,
    const GLenum                    wrapMethod,
    const BOOL                      mipmap)
    : Texture(TEXTURE_CUBEBOX)
{
    assert(textureFile.size() == CUBE_MAP_FACE_NUM);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);

    for (size_t i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        m_cubeMapData[i] = stbi_load(textureFile[i].data(),
                                     reinterpret_cast<int*>(&m_textureWidth),
                                     reinterpret_cast<int*>(&m_textureHeight),
                                     reinterpret_cast<int*>(&m_textureDataType),
                                     STBI_rgb_alpha);

        glTexImage2D(
            cubeFace[i], 0, format, m_textureWidth, m_textureHeight, 0, format, type, m_cubeMapData[i]);

        if (mipmap == TRUE)
        {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMethod);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMethod);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapMethod);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::~Cubemap()
{
    for (void* texData : m_cubeMapData)
    {
        stbi_image_free(texData);
    }
}

void Cubemap::bindTexture(
    const GLenum       textureUnit,
    const GLuint       shaderProgram,
    const std::string& samplerName,
    const int          samplerIndex)
{
    GLint textureLocation = glGetUniformLocation(shaderProgram, samplerName.data());

    assert(textureLocation >= 0);

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);
    glUniform1i(textureLocation, samplerIndex);
}

void Cubemap::unbindTexture()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
