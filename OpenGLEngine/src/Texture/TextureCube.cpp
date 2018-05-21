#include "Texture.h"

TextureCube::TextureCube(const std::string & textureFile,
                         GLenum format, GLenum type,
                         GLenum wrapMethod, BOOL mipmap)
    : Texture(TEXTURE_CUBEBOX)
{
}

TextureCube::~TextureCube()
{
}
