//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

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
                m_cubeMapData[i].release();
            }
        }
    }
}