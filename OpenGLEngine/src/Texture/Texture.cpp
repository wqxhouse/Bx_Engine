#include "Texture.h"

Texture::Texture(TextureType textureType)
    : m_textureType(textureType)
{
	glGenTextures(1, &m_textureHandle);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureHandle);
}
