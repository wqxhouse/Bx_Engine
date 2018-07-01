#pragma once

#include "../Core/TypeDef.h"

enum PolyMode
{
	WIREFRAME = 0x00000000,
	TRIANGLE  = 0x00000001,
};

enum RenderingMethod
{
    FORWARD_RENDERING,
    DEFERRED_RENDERING,
};

enum Antialasing
{
    NONE  = 0x00000000,
    LOW   = 0x00000002,
    HIGH  = 0x00000004,
    ELTRA = 0x00000008,
};

struct AmbientOcclutionSetting
{
    enum AmbientOcclusion
    {
        NONE, SSAO,
    };

    enum SSAO_Quality
    {
        LOW    = 0x00000004,
        NORMAL = 0x0000000F,
        HIGH   = 0x00000040,
        ELTRA  = 0x000000F0,
    };

    AmbientOcclusion ambientOcclusion;
    SSAO_Quality     ssaoQuality;
};

struct GraphicsSetting
{
    RenderingMethod renderingMethod = RenderingMethod::FORWARD_RENDERING;
    Antialasing antialasing         = Antialasing::HIGH;
    BOOL shadowCasting              = TRUE;

    inline void EnableSSAO()
    {
        ambientOcclutionSetting.ambientOcclusion =
            AmbientOcclutionSetting::AmbientOcclusion::SSAO;
    }

    inline void DisableSSAO()
    {
        ambientOcclutionSetting.ambientOcclusion =
            AmbientOcclutionSetting::AmbientOcclusion::NONE;
    }

    AmbientOcclutionSetting ambientOcclutionSetting = 
    { 
        AmbientOcclutionSetting::AmbientOcclusion::SSAO, 
        AmbientOcclutionSetting::SSAO_Quality::HIGH
    };
};

struct Setting
{
	unsigned int width = 1280;
    unsigned int height = 720;

	float fov = 45.0f;

	PolyMode polyMode = PolyMode::TRIANGLE;

	GraphicsSetting m_graphicsSetting;
};