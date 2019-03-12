//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/TypeDef.h"

struct AmbientOcclutionSetting
{
    AmbientOcclusion ambientOcclusion;
    SSAO_Quality     ssaoQuality;
};

struct GraphicsSetting
{
    RenderingMethod renderingMethod = RenderingMethod::FORWARD_RENDERING;
    Antialasing     antialasing     = Antialasing::AA_NONE;
    Anisotropy      anisotropy      = Anisotropy::ANISOTROPY_FOUR;
    BOOL            blend           = FALSE;

    BOOL shadowCasting;
    Resolution shadowResolution     = { 128, 128 };

    INLINE void EnableSSAO()
    {
        ambientOcclutionSetting.ambientOcclusion = AmbientOcclusion::AMBIENTOCCLUSION_SSAO;
    }

    INLINE void DisableSSAO()
    {
        ambientOcclutionSetting.ambientOcclusion = AmbientOcclusion::AMBIENTOCCLUSION_NONE;
    }

    INLINE BOOL IsEnableAntialasing() const
    {
        return ((antialasing != AA_NONE) == TRUE);
    }

    AmbientOcclutionSetting ambientOcclutionSetting =
    {
        AmbientOcclusion::AMBIENTOCCLUSION_SSAO,
        SSAO_Quality::SSAO_HIGH
    };
};

struct Setting
{
    GraphicsSetting m_graphicsSetting;

    Resolution resolution = { 1280, 720 };
    float fov             = 45.0f;

    PolyMode polyMode = PolyMode::POLYMODE_FILL;

    BOOL vsync      = TRUE;
    BOOL fullScreen = FALSE;

    UINT maxSceneNum = DEFAULT_MAX_RENDER_SCENE_NUM;
};