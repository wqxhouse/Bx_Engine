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
    struct TextureSamplerCreateData
    {
        TextureSamplerFilter         magFilter;
        TextureSamplerFilter         minFilter;
        TextureSamplerAddressingMode addressingModeU;
        TextureSamplerAddressingMode addressingModeV;
        TextureSamplerAddressingMode addressingModeW;
        float                        anisotropyNum;
        Math::Vector4                borderColor;
        BOOL                         normalize;
        TextureSamplerFilter         mipmapFilter;
        float                        mipmapOffset;
        float                        minLod;
        float                        maxLod;
    };

    struct TextureCreateData
    {
        UINT                      texWidth;
        UINT                      texHeight;
        UINT                      samples;
        BOOL                      mipmap;
        TextureFormat             texLoadFormat;
        TextureFormat             texStoreFormat;
        TextureUsage              texUsage;
        BOOL                      texOptimize;
        BOOL                      texPerserve;
        TextureSamplerCreateData* pSampleCreateData = NULL;
    };

    struct TextureDeleter
    {
        void operator()(void* ptr)
        {
            free(ptr);
        }
    };

    class TextureBase
    {
    public:
        TextureBase(
            const TextureType        textureType,
            const TextureCreateData* pTextureCreateData);

        ~TextureBase();

        INLINE TextureType   GetTextureType()        const { return m_textureType;   }

        INLINE UINT          GetTextureWidth()       const { return m_textureWidth;  }
        INLINE UINT          GetTextureHeight()      const { return m_textureHeight; }

        INLINE UINT          GetSampleNumber()       const { return m_samples;       }

        INLINE TextureUsage  GetTextureUsage()       const { return m_usage;         }
        INLINE TextureFormat GetTextureFormat()      const { return m_storeFormat;   }

        INLINE BOOL          IsGenMipmap()           const { return m_mipmap;        }
        INLINE BOOL          IsTextureOptimize()     const { return m_texOptimize;   }
        INLINE BOOL          IsTextureDataPerserve() const { return m_texPerserve;   }

        static std::unique_ptr<image_data, TextureDeleter> ReadImageData(
                const std::string& imageFile,
                int* const         width,
                int* const         height,
                int* const         channels);

    protected:
        UINT  m_textureWidth;
        UINT  m_textureHeight;

        UINT  m_samples;
        BOOL  m_mipmap;

        TextureFormat m_loadFormat;
        TextureFormat m_storeFormat;
        TextureUsage  m_usage;

        BOOL  m_texOptimize;
        BOOL  m_texPerserve;

    private:
        TextureType m_textureType;
    };
}
