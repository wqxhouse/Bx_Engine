#pragma once

#include "TextureBase.h"

#define CUBE_MAP_FACE_NUM 6

namespace Texture
{
    struct CubemapCreateData : TextureCreateData
    {
        std::array<std::unique_ptr<image_data>, CUBE_MAP_FACE_NUM> cubemapData;
    };

    class Cubemap : public TextureBase
    {
    public:
        explicit Cubemap(
            CubemapCreateData* pCreateData);
        ~Cubemap();

        inline void unbindTexture();

    private:
        std::array<std::unique_ptr<image_data>, CUBE_MAP_FACE_NUM> m_cubeMapData;
    };
}