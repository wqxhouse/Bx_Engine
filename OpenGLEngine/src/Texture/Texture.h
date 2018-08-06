#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Shader/Shader.h"

#define CUBE_MAP_FACE_NUM 6

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

    inline TextureType GetTextureType()   const { return m_textureType;   }
    inline GLuint      GetTextureHandle() const { return m_textureHandle; }

    inline UINT        GetTextureWidth()  const { return m_textureWidth;  }
    inline UINT        GetTextureHeight() const { return m_textureHeight; }

    inline void SetTextureHandle(
        const GLuint textureHandle) 
    {
        m_textureHandle = textureHandle;
    }

protected:
    GLuint m_textureHandle;

    UINT  m_textureDataType;

    UINT  m_textureWidth;
    UINT  m_textureHeight;

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
        const GLenum wrapMethod  = GL_CLAMP_TO_BORDER,
        const BOOL   mipmap      = FALSE,
        const void*  data        = NULL);

    // Create 2D texture from texture file
    Texture2D(
        const std::string& textureFile,
        const GLenum format     = GL_RGBA,
        const GLenum type       = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod = GL_CLAMP_TO_BORDER,
        const BOOL   mipmap     = GL_FALSE);

    void setBoarderColor(GLfloat borderColor[4]);

    void bindTexture(
        const GLenum       textureUnit,
        const GLuint       shaderProgram,
        const std::string& samplerName,
        const int          samplerIndex);

    void setTextureSampleMethod(
        const GLenum minSampleMethod,  // Nearest neighbor / Linear
        const GLenum magSampleMethod); // Nearest neighbor / Linear

    inline void unbindTexture();

    inline void* getTextureData();

    ~Texture2D();

private:
    UINT  m_samples;
    BOOL  m_mipmap;

    void* m_textureData;
};

class Texture3D : public Texture
{
public:
    Texture3D(
        const std::string& textureFile,
        const GLenum format     = GL_RGBA,
        const GLenum type       = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod = GL_CLAMP_TO_BORDER,
        const BOOL mipmap       = GL_FALSE);

    ~Texture3D();

    void bindTexture(
        const GLenum       textureUnit,
        const GLuint       shaderProgram,
        const std::string& samplerName,
        const int          samplerIndex);
};

class Cubemap : public Texture
{
public:
    Cubemap(
        const UINT   texSize,
        const UINT   samples     = 1,
        const GLenum loadFormat  = GL_RGBA,
        const GLenum storeFormat = GL_RGBA,
        const GLenum type        = GL_UNSIGNED_BYTE,
        const GLenum wrapMethod  = GL_CLAMP_TO_EDGE,
        const BOOL   mipmap      = FALSE,
        const void*  data        = NULL);

    Cubemap(
        const std::vector<std::string>& textureFile,
        const GLenum                    format     = GL_RGBA,
        const GLenum                    type       = GL_UNSIGNED_BYTE,
        const BOOL   mipmap                        = FALSE);

    ~Cubemap();

    void bindTexture(
        const GLenum       textureUnit,
        const GLuint       shaderProgram,
        const std::string& samplerName,
        const int          samplerIndex);

    void update(
        const std::vector<std::string>& textureFile,
        const GLenum                    format = GL_RGBA,
        const GLenum                    type   = GL_UNSIGNED_BYTE);

    inline void unbindTexture();

private:
    void* m_cubeMapData[6];
};
