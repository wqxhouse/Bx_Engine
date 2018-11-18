#pragma once

#include "Texture2D.h"

namespace Texture
{
    struct Texture3DCreateData : TextureCreateData
    {
        UINT                                    texLayers;
        TextureFormat                           texLoadFormat;
        TextureFormat                           texStoreFormat;
        std::vector<std::unique_ptr<image_data>> textureData;
    };

    class Texture3D : public TextureBase
    {
    public:
        explicit Texture3D(
            Texture3DCreateData* pCreateData);

        ~Texture3D();

        inline void unbindTexture();

        inline UINT GetTextureLayer() const { return m_textureLayer; }

    private:
        UINT                                    m_textureLayer;
        std::vector<std::unique_ptr<image_data>> m_texturesData;
    };
}