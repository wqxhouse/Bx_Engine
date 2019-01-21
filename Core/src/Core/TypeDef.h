//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define INLINE  __forceinline

#define BOOL    uint32_t
#define BOOL_F  float
#define BYTE    int8_t
#define UBYTE   uint8_t
#define INT16   int16_t
#define INT     int32_t
#define INT64   int64_t
#define UINT16  uint16_t
#define UINT    uint32_t
#define UINT64  uint64_t

#define TRUE    1
#define FALSE   0

#define BX_SUCCESS 1
#define BX_FAIL    0

#define TRUE_F  (1.0F)
#define FALSE_F (0.0F)

#define INVALID_HANDLE 0xFFFFFFFF
#define INVALID_INDEX  0xFFFFFFFF

#define DEFAULT_MAX_RENDER_SCENE_NUM 16

#define IN
#define OUT

#define DEBUG

#define DEFAULT_SHADER_PATH "../Resources/shaders/"

#define CLAMP(v, l, r) \
	((v) = (((v) < (l)) ? (l) : (v))); \
	((v) = (((v) > (r)) ? (r) : (v))); \

typedef unsigned char  image_data;
typedef unsigned char* image_ptr;

template<typename T>
INLINE void SafeFree(T* ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

template<typename T>
INLINE void SafeDelete(T* ptr)
{
    if (ptr != NULL)
    {
        delete ptr;
    }
}

INLINE void SafeDeleteArray(void *ptr)
{
    if (ptr != NULL)
    {
        delete[] ptr;
    }
}

INLINE void NotSupported()
{
    printf("This feature is not supported.\n");
    assert(FALSE);
}

INLINE void NotImplemented()
{
    printf("This feature hasn't been implemented yet.\n");
    assert(FALSE);
}

enum PolyMode
{
    POLYMODE_POINT     = 0x00000000,
    POLYMODE_WIREFRAME = 0x00000001,
    POLYMODE_FILL      = 0x00000002
};

enum RenderingMethod
{
    // Normal rendering
    FORWARD_RENDERING,
    DEFERRED_RENDERING,

    // Tile based rendering
    FORWARD_PLUS_RENDERING,
    DEFERRED_TILED_RENDERING,

    // TODO: Clustered rendering
    FORWARD_CLUSTERED_RENDERING,
    DEFERRED_CLUSTERED_RENDERING
};

enum Antialasing
{
    AA_NONE = 0x00000000,
    AA_LOW = 0x00000002,
    AA_HIGH = 0x00000004,
    AA_EXTRA = 0x00000008,
};

enum AmbientOcclusion
{
    AMBIENTOCCLUSION_NONE,
    AMBIENTOCCLUSION_SSAO,
    // TODO: HBAO, HBAO+, VXAO
};

enum SSAO_Quality
{
    SSAO_LOW    = 0x00000004,
    SSAO_NORMAL = 0x0000000F,
    SSAO_HIGH   = 0x00000040,
    SSAO_ELTRA  = 0x000000F0,
};

enum VertexElement
{
    POSITION,
    NORMAL,
    TANGENT,
    TEXTURE,
    TEXTURE1,
    TEXTURE2,
    TEXTURE3,
    TEXTURE4,
    TEXTURE5,
    TEXTURE6,
    TEXTURE7,
    TEXTURE8,
    TEXTURE9
};

enum TextureType
{
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBEMAP
};

enum TextureFormat
{
    BX_FORMAT_NONE = 0,
    BX_FORMAT_R8,
    BX_FORMAT_RG8,
    BX_FORMAT_RGB8,
    BX_FORMAT_RGBA8,
    BX_FORMAT_RGBA16,
    BX_FORMAT_RGBA32,
    BX_FORMAT_RGBA64,
    BX_FORMAT_RGBA,
    BX_FORMAT_SRGB,
    BX_FORMAT_DEPTH16,
    BX_FORMAT_DEPTH16_STENCIL,
    BX_FORMAT_DEPTH24_STENCIL,
    BX_FORMAT_DEPTH32,
    BX_FORMAT_DEPTH32_STENCIL,
    BX_FORMAT_MAX = 0x7FFFFFFF
};

enum TextureUsage
{
    BX_TEXTURE_USAGE_NONE                     = 0x0,
    BX_TEXTURE_USAGE_SAMPLED                  = 0x1,
    BX_TEXTURE_USAGE_RENDER_TARGET            = 0x2,
    BX_TEXTURE_USAGE_COLOR_ATTACHMENT         = 0x4,
    BX_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x8,
    BX_TEXTURE_USAGE_MAX                      = 0x7FFFFFFF
};

enum TextureSamplerFilter
{
    BX_TEXTURE_SAMPLER_FILTER_NEAREST = 0x0,
    BX_TEXTURE_SAMPLER_FILTER_LINEAR  = 0x1,
    BX_TEXTURE_SAMPLER_FILTER_CUBIC   = 0x2,
    BX_TEXTURE_SAMPLER_FILTER_MAX     = 0x7FFFFFFF
};

enum TextureSamplerAddressingMode
{
    BX_TEXTURE_SAMPLER_ADDRESSING_REPEAT,
    BX_TEXTURE_SAMPLER_ADDRESSING_MIRROR_REPEAT,
    BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_EDGE,
    BX_TEXTURE_SAMPLER_ADDRESSING_CLAMP_TO_BORDER,
    BX_TEXTURE_SAMPLER_ADDRESSING_MAX = 0x7FFFFFFF
};

enum MESH_TEXTURE_MAP_TYPES
{
    DIFFUSE_MAP       = 0,
    SPECULAR_MAP      = 1,
    NORMAL_MAP        = 2,
    BUMP_MAP          = 3,
    LIGHT_MAP         = 4,
    MESH_TEXTURE_SIZE = 5
};

enum LightType
{
    DIRECTIONAL_LIGHT = 0x0,
    POINT_LIGHT = 0x1,
    SPOT_LIGHT = 0x2,
    AREA_LIGHT = 0x3,
    UNKNOWN_LIGHT = 0xffffffff
};

enum CameraType
{
    PROSPECTIVE_CAM,
    ORTHOGRAPHIC_CAM
};

enum MaterialType
{
    PHONG,
    ALBEDO,
    COOKTORRANCE
};

enum ShadingMethod
{
    Flat,
    Gouraud,
    Phong,
    Blin_Phong,
    PBR_LAMBERT,
    PBR_COOK_TORRANCE
};

enum ModelFormat
{
    OBJ,
    FBX
};

struct Resolution
{
    UINT width;
    UINT height;
};
