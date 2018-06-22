#pragma once

#include "../Core/OpenGLPCH.h"

enum TextureType
{
    TEXTURE_2D, TEXTURE_3D, TEXTURE_CUBEBOX
};

class Texture
{
public:
    Texture(TextureType textureType);
    
    virtual void bindTexture(const GLenum textureIndex,
                             const GLuint shaderProgram,
                             const std::string& samplerName,
                             const int index) = 0;

    virtual void unbindTexture() = 0;

    virtual ~Texture();

    inline TextureType GetTextureType()   const { return m_textureType; }
    inline GLuint      GetTextureHandle() const { return m_textureHandle; }

protected:
    GLuint m_textureHandle;

private:
    TextureType m_textureType;

};

class Texture2D : public Texture
{
public:
    // Create empty 2D texture
    Texture2D(
        const UINT   texWidth,
        const UINT   texHeight,
        const UINT   samples     = 1,
        const GLenum loadFormat  = GL_RGBA,
        const GLenum storeFormat = GL_RGBA,
        const GLenum type        = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod  = GL_REPEAT,
        const BOOL   mipmap      = FALSE,
        const void*  data        = NULL);

    // Create 2D texture from texture file
    Texture2D(
        const std::string& textureFile,
        const GLenum format     = GL_RGBA,
        const GLenum type       = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod = GL_REPEAT,
        const BOOL   mipmap     = GL_FALSE);

    void setBoarderColor(GLfloat borderColor[4]);

    void bindTexture(
        const GLenum textureUnit,
        const GLuint shaderProgram,
        const std::string& samplerName,
        const int samplerIndex);

    inline void unbindTexture();

    inline void* getTextureData();

    ~Texture2D();

private:
    UINT  m_textureHeight;
    UINT  m_textureWidth;

    UINT  m_samples;

    UINT  m_textureType;

    void* m_textureData;
};

class Texture3D : public Texture
{
public:
    Texture3D(
        const std::string& textureFile,
        const GLenum format     = GL_RGBA,
        const GLenum type       = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod = GL_REPEAT,
        const BOOL mipmap       = GL_FALSE);

    ~Texture3D();
};

class TextureCube : public Texture
{
public:
    TextureCube(
        const std::string& textureFile,
        const GLenum format     = GL_RGBA,
        const GLenum type       = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod = GL_REPEAT,
        const BOOL mipmap       = GL_FALSE);

    ~TextureCube();
};
