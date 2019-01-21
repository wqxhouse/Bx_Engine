//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Core/TypeDef.h>

#define FRUSTUM_GRID_SSBO_BINDING_POINT 0
#define LIGHT_GRID_SSBO_BINDING_POINT   1
#define LIGHT_INDEX_LIST_BINDING_POINT  2
#define LIGHT_BUFFER_SSBO_BINDING_POINT 3

enum BX_SWAPCHAIN_SURFACE_BUFFER
{
    BX_SWAPCHAIN_SURFACE_BUFFER_SINGLE,
    BX_SWAPCHAIN_SURFACE_BUFFER_DOUBLE,
    BX_SWAPCHAIN_SURFACE_BUFFER_TRIPLE
};

enum BX_SHADER_TYPE
{
    BX_VERTEX_SHADER   = 0,
    BX_TES_SHADER      = 1,
    BX_TCS_SHADER      = 2,
    BX_GEOMETRY_SHADER = 3,
    BX_FRAGMENT_SHADER = 4,
    BX_MAX_SHADER      = 5,

    BX_COMPUTE_SHADER  = 6,
};

enum BX_QUEUE_TYPE
{
    BX_QUEUE_GRAPHICS = 0,
    BX_QUEUE_COMPUTE  = 1,
    BX_QUEUE_PRESENT  = 2
};

enum BX_COMMAND_BUFFER_TYPE
{
    BX_GRAPHICS_COMMAND_BUFFER = 0,
    BX_COMPUTE_COMMAND_BUFFER  = 1
};

enum BX_COMMAND_BUFFER_LEVLE
{
    BX_DIRECT_COMMAND_BUFFER   = 0,
    BX_INDIRECT_COMMAND_BUFFER = 1
};

enum BX_VERTEX_INPUT_RATE
{
    BX_VERTEX_INPUT_RATE_VERTEX,
    BX_VERTEX_INPUT_RATE_INSTANCE
};

enum BX_INDEX_TYPE
{
    BX_INDEX_UINT16,
    BX_INDEX_UINT32
};

enum BX_DESCRIPTOR_TYPE
{
    BX_UNIFORM_DESCRIPTOR         = 0,
    BX_SAMPLER_DESCRIPTOR         = 1,
    BX_STORAGE_BUFFER_DESCRIPTOR  = 2,
};

enum BX_FRAMEBUFFER_ATTACHMENT_LAYOUT
{
    BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_COLOR,
    BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_DEPTH_STENCIL,
    BX_FRAMEBUFFER_ATTACHMENT_LAYOUT_PRESENT
};