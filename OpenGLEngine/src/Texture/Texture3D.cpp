#include "Texture.h"

Texture3D::Texture3D(const std::string & textureFile, GLenum format,
                     GLenum type, GLenum wrapMethod, BOOL mipmap)
    : Texture(TEXTURE_3D)
{
}

Texture3D::~Texture3D()
{
}