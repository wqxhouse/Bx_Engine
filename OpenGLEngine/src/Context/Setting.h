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
    Antialasing antialasing         = Antialasing::AA_HIGH;

    BOOL shadowCasting;
    Resolution shadowResolution     = { 128, 128 };

    inline void EnableSSAO()
    {
        ambientOcclutionSetting.ambientOcclusion = AmbientOcclusion::AMBIENTOCCLUSION_SSAO;
    }

    inline void DisableSSAO()
    {
        ambientOcclutionSetting.ambientOcclusion = AmbientOcclusion::AMBIENTOCCLUSION_NONE;
    }

    AmbientOcclutionSetting ambientOcclutionSetting = 
    { 
        AmbientOcclusion::AMBIENTOCCLUSION_SSAO,
        SSAO_Quality::SSAO_HIGH
    };
};

struct Setting
{
    Resolution resolution = { 1280, 720 };

	float fov = 45.0f;

	PolyMode polyMode = PolyMode::POLYMODE_FILL;

    BOOL vsync = TRUE;

    BOOL fullScreen = FALSE;

	GraphicsSetting m_graphicsSetting;
};