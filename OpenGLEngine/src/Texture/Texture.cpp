//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

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
