#pragma once

#include "../Core/OpenGLPCH.h"

class Texture
{
public:
	Texture(const std::string& textureFile,
		    GLenum format = GL_RGBA,
		    GLenum type = GL_UNSIGNED_BYTE,
		    GLboolean mipmap = GL_FALSE);
	
	virtual void bindTexture(const GLuint shaderProgram, const std::string& samplerName, const int index) = 0;
	virtual void unbindTexture() = 0;

	virtual ~Texture();

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

	void bindTexture(const GLuint textureIndex,
		             const GLuint shaderProgram,
		             const std::string& samplerName,
		             const int samplerIndex);
	void unbindTexture();

	virtual ~Texture2D();

private:
	UINT  m_textureHeight;
	UINT  m_textureWidth;
	UINT  m_textureType;

	void* m_textureData;
};