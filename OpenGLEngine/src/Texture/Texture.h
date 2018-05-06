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

    TextureType m_textureType;

protected:
    GLuint m_textureHandler;
};

class Texture2D : public Texture
{
public:
    Texture2D(const std::string& textureFile,
              GLenum format     = GL_RGBA,
              GLenum type       = GL_UNSIGNED_BYTE,
              GLenum wrapMethod = GL_REPEAT,
              GLboolean mipmap  = GL_FALSE);

    void setBoarderColor(GLfloat borderColor[4]);

    void bindTexture(const GLenum textureIndex,
                     const GLuint shaderProgram,
                     const std::string& samplerName,
                     const int samplerIndex);

    inline void unbindTexture();
    
    inline void* getTextureData();

    ~Texture2D();

private:
    UINT  m_textureHeight;
    UINT  m_textureWidth;
    UINT  m_textureType;

    void* m_textureData;
};

class Texture3D : public Texture
{
public:
    Texture3D(const std::string& textureFile,
              GLenum format = GL_RGBA,
              GLenum type = GL_UNSIGNED_BYTE,
              GLenum wrapMethod = GL_REPEAT,
              GLboolean mipmap = GL_FALSE);
    ~Texture3D();
};

class TextureCube : public Texture
{
public:
    TextureCube(const std::string& textureFile,
                GLenum format = GL_RGBA,
                GLenum type = GL_UNSIGNED_BYTE,
                GLenum wrapMethod = GL_REPEAT,
                GLboolean mipmap = GL_FALSE);
    ~TextureCube();
};
