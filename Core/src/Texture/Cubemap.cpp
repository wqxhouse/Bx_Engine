#include "Cubemap.h"

namespace Texture
{
    Cubemap::Cubemap(
        CubemapCreateData * pCreateData)
        : TextureBase(TEXTURE_CUBEMAP, static_cast<TextureCreateData*>(pCreateData))
    {
        m_cubeMapData = std::move(pCreateData->cubemapData);
    }

    Cubemap::~Cubemap()
    {
        for (UINT i = 0; i < CUBE_MAP_FACE_NUM; ++i)
        {
            if (m_cubeMapData[i] != NULL)
            {
                Utility::UtilityBase::ReleaseImageData(std::move(m_cubeMapData[i]));
            }
        }
    }
}