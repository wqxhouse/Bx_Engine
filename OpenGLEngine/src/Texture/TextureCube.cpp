#include "Texture.h"

TextureCube::TextureCube(const std::string & textureFile,
                         GLenum format, GLenum type,
                         GLenum wrapMethod, GLboolean mipmap)
    : Texture(TEXTURE_CUBEBOX)
{
}