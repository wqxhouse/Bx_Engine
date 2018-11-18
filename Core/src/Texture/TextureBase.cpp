#include "TextureBase.h"

namespace Texture
{
    TextureBase::TextureBase(
        const TextureType        textureType,
        const TextureCreateData* pTextureCreateData)
        : m_textureType(textureType)
    {
        m_textureWidth = pTextureCreateData->texWidth;
        m_textureHeight = pTextureCreateData->texHeight;
        m_samples = pTextureCreateData->samples;
        m_mipmap = pTextureCreateData->mipmap;
    }

    TextureBase::~TextureBase()
    {
    }
}
