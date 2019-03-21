//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "TextureBase.h"

namespace Texture
{
    struct Texture2DCreateData : TextureCreateData
    {
        std::unique_ptr<image_data, TextureDeleter> textureData;
    };

    class Texture2D : public TextureBase
    {
    public:
        explicit Texture2D(
            Texture2DCreateData* pCreateData);

        ~Texture2D();

        INLINE const void* const GetTextureData() const
        {
            return m_textureData.get();
        }

        INLINE void FreeTextureData()
        {
            m_textureData.release();
        }

    private:
        std::unique_ptr<image_data, TextureDeleter> m_textureData;
    };
}