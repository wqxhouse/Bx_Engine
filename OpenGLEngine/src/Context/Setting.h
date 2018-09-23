#pragma once

#include "../Core/TypeDef.h"

struct Resolution
{
    UINT width;
    UINT height;
};

struct AmbientOcclutionSetting
{
    AmbientOcclusion ambientOcclusion;
    SSAO_Quality     ssaoQuality;
};

struct GraphicsSetting
{
    RenderingMethod renderingMethod = RenderingMethod::FORWARD_RENDERING;
    Antialasing antialasing         = Antialasing::AA_HIGH;

    BOOL shadowCasting              = TRUE;
    Resolution shadowResolution     = { 4096, 4096 };

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

	GraphicsSetting m_graphicsSetting;
};