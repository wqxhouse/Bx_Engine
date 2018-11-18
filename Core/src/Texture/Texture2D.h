#pragma once

#include "TextureBase.h"

namespace Texture
{
    struct Texture2DCreateData : TextureCreateData
    {
        std::unique_ptr<image_data> textureData;
    };

    class Texture2D : public TextureBase
    {
    public:
        explicit Texture2D(
            Texture2DCreateData* pCreateData);

        ~Texture2D();

        inline const void* const GetTextureData() const
        {
            return m_textureData._Myptr();
        }

    private:
        std::unique_ptr<image_data> m_textureData;
    };
}