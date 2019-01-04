//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Texture3D.h"

namespace Texture
{
    Texture3D::Texture3D(
        Texture3DCreateData * pCreateData)
        : TextureBase(TEXTURE_3D, static_cast<TextureCreateData*>(pCreateData))
    {
        m_textureLayer = pCreateData->texLayers;
        m_loadFormat   = pCreateData->texLoadFormat;
        m_storeFormat  = pCreateData->texStoreFormat;
        m_texturesData = std::move(pCreateData->textureData);
    }

    Texture3D::~Texture3D()
    {
        for (UINT i = 0; i < m_textureLayer; ++i)
        {
            if (m_texturesData[i] != NULL)
            {
                m_texturesData[i].release();
            }
        }
    }
}