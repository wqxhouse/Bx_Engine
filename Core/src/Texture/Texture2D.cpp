//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Texture2D.h"

namespace Texture
{
    Texture2D::Texture2D(
        Texture2DCreateData* pCreateData)
        : TextureBase(TEXTURE_2D, static_cast<TextureCreateData*>(pCreateData))
    {
        m_loadFormat  = pCreateData->texLoadFormat;
        m_storeFormat = pCreateData->texStoreFormat;
        m_textureData = std::move(pCreateData->textureData);
    }

    Texture2D::~Texture2D()
    {
        if (m_textureData != NULL)
        {
            Utility::UtilityBase::ReleaseImageData(std::move(m_textureData));
        }
    }
}