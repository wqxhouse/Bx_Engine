#pragma once

#include <stdint.h>

#define BOOL    uint32_t
#define BOOL_F  float
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

#define FRUSTUM_GRID_SSBO_BINDING_POINT 0
#define LIGHT_GRID_SSBO_BINDING_POINT   1
#define LIGHT_INDEX_LIST_BINDING_POINT  2
#define LIGHT_BUFFER_SSBO_BINDING_POINT 3

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

enum MESH_TEXTURE_MAP_TYPES
{
    DIFFUSE_MAP       = 0,
    SPECULAR_MAP      = 1,
    NORMAL_MAP        = 2,
    BUMP_MAP          = 3,
    LIGHT_MAP         = 4,
    MESH_TEXTURE_SIZE = 5
};

enum FboRenderbufferAttachmentType
{
    FRAMEBUFFER_COLOR_RENDERBUFFER_ATTACHMENT         = 0,
    FRAMEBUFFER_DEPTH_RENDERBUFFER_ATTACHMENT         = 1,
    FRAMEBUFFER_STENCIL_RENDERBUFFER_ATTACHMENT       = 2,
    FRAMEBUFFER_DEPTH_STENCIL_RENDERBUFFER_ATTACHMENT = 3
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