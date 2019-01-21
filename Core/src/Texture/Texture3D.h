//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "Texture2D.h"

namespace Texture
{
    struct Texture3DCreateData : TextureCreateData
    {
        UINT                                                     texLayers;
        TextureFormat                                            texLoadFormat;
        TextureFormat                                            texStoreFormat;
        std::vector<std::unique_ptr<image_data, TextureDeleter>> textureData;
    };

    class Texture3D : public TextureBase
    {
    public:
        explicit Texture3D(
            Texture3DCreateData* pCreateData);

        ~Texture3D();

        INLINE void unbindTexture();

        INLINE UINT GetTextureLayer() const { return m_textureLayer; }

    private:
        UINT                                                     m_textureLayer;
        std::vector<std::unique_ptr<image_data, TextureDeleter>> m_texturesData;
    };
}