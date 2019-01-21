//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

//Image loader
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include "TextureBase.h"

namespace Texture
{
    TextureBase::TextureBase(
        const TextureType        textureType,
        const TextureCreateData* pTextureCreateData)
        : m_textureType(textureType)
    {
        m_textureWidth  = pTextureCreateData->texWidth;
        m_textureHeight = pTextureCreateData->texHeight;
        m_usage         = pTextureCreateData->texUsage;
        m_loadFormat    = pTextureCreateData->texLoadFormat;
        m_storeFormat   = pTextureCreateData->texStoreFormat;
        m_samples       = pTextureCreateData->samples;
        m_mipmap        = pTextureCreateData->mipmap;
        m_texOptimize   = pTextureCreateData->texOptimize;
        m_texPerserve   = pTextureCreateData->texPerserve;
    }

    TextureBase::~TextureBase()
    {
    }

    std::unique_ptr<image_data, TextureDeleter> TextureBase::ReadImageData(
        const std::string& imageFile,
        int* const         width,
        int* const         height,
        int* const         channels)
    {
        std::unique_ptr<image_data, TextureDeleter> imageData(
            stbi_load(imageFile.data(), width, height, channels, STBI_rgb_alpha));

        return imageData;
    }
}
