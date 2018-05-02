#include "Texture.h"

Texture::Texture(const std::string& textureFile, GLenum format, GLenum type, GLboolean mipmap)
{
	glGenTextures(1, &m_textureHandler);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureHandler);
}

