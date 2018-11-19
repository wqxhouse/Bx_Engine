//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/PCH.h"

namespace Texture
{
    struct TextureCreateData
    {
        UINT                  texWidth;
        UINT                  texHeight;
        UINT                  samples;
        BOOL                  mipmap;
        TextureFormat         texLoadFormat;
        TextureFormat         texStoreFormat;
    };

    class TextureBase
    {
    public:
        TextureBase(
            const TextureType        textureType,
            const TextureCreateData* pTextureCreateData);

        ~TextureBase();

        inline TextureType GetTextureType()   const { return m_textureType; }

        inline UINT        GetTextureWidth()  const { return m_textureWidth; }
        inline UINT        GetTextureHeight() const { return m_textureHeight; }

        inline UINT        GetSampleNumber()  const { return m_samples; }

    protected:
        UINT  m_textureWidth;
        UINT  m_textureHeight;

        UINT  m_samples;
        BOOL  m_mipmap;

        TextureFormat m_loadFormat;
        TextureFormat m_storeFormat;

    private:
        TextureType m_textureType;
    };
}
