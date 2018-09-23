#include "Texture.h"

#include "stb_image.h"

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
        const UINT   texSize,
        const UINT   samples,
        const GLenum loadFormat,
        const GLenum storeFormat,
        const GLenum type,
        const GLenum wrapMethod,
        const BOOL   mipmap,
        const void*  data)
    : Texture(TEXTURE_CUBEMAP)
{
    m_textureWidth = m_textureHeight = texSize;

    m_samples = samples;
    m_mipmap  = mipmap;

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);

    for (size_t i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        m_cubeMapData[i] = NULL;

        glTexImage2D(
            cubeFace[i], 0, storeFormat, m_textureWidth, m_textureHeight, 0, loadFormat, type, NULL);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMethod);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMethod);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapMethod);
    }

    if (mipmap == TRUE)
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::Cubemap(
    const std::vector<std::string>& textureFile,
    const UINT                      samples,
    const GLenum                    format,    
    const GLenum                    type,
    const BOOL                      mipmap)
    : Texture(TEXTURE_CUBEMAP)
{
    assert(textureFile.size() == CUBE_MAP_FACE_NUM);

    m_samples = samples;
    m_mipmap  = mipmap;

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);

    for (size_t i = 0; i < CUBE_MAP_FACE_NUM; ++i)
    {
        m_cubeMapData[i] = stbi_load(textureFile[i].data(),
                                     reinterpret_cast<int*>(&m_textureWidth),
                                     reinterpret_cast<int*>(&m_textureHeight),
                                     reinterpret_cast<int*>(&m_textureDataType),
                                     STBI_rgb_alpha);

        // The width and height for cubemap must be the same
        assert(m_textureWidth == m_textureHeight);

        glTexImage2D(cubeFace[i], 0, format, m_textureWidth, m_textureHeight, 0, format, type, m_cubeMapData[i]);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    if (mipmap == TRUE)
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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
    const std::string& samplerName)
{
    GLint textureLocation = glGetUniformLocation(shaderProgram, samplerName.data());

    assert(textureLocation >= 0);

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);
    glUniform1i(textureLocation, textureUnit - GL_TEXTURE0);
}

void Cubemap::bindTexture(
    const GLenum       textureUnit,
    const GLuint       shaderProgram,
    const std::string& samplerName,
    const int          bindPosition)
{
    GLint textureLocation = glGetUniformLocation(shaderProgram, samplerName.data());

    assert(textureLocation >= 0);

    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);
    glUniform1i(textureLocation, textureUnit - GL_TEXTURE0);
}

void Cubemap::update(
    const std::vector<std::string>& textureFile,
    const GLenum                    format,
    const GLenum                    type)
{
    for (void* texData : m_cubeMapData)
    {
        stbi_image_free(texData);
    }

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
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::unbindTexture()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
