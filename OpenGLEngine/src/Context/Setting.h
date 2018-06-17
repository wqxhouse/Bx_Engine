#pragma once

#include "../Core/TypeDef.h"

enum PolyMode
{
	WIREFRAME = 0x00000000,
	TRIANGLE = 0x00000001,
};

enum RenderingMethod
{
    FORWARD_RENDERING,
    DEFERRED_RENDERING,
};

enum Antialasing
{
    NONE = 0x00000000,
    LOW = 0x00000002,
    HIGH = 0x00000004,
    ELTRA = 0x00000008,
};

struct GraphicsSetting
{
    Antialasing antialasing         = Antialasing::HIGH;
    RenderingMethod renderingMethod = RenderingMethod::FORWARD_RENDERING;
    BOOL shadowCasting              = TRUE;
};

struct Setting
{
	unsigned int width = 1280;
    unsigned int height = 720;

	float fov = 45.0f;

	PolyMode polyMode = PolyMode::TRIANGLE;

	GraphicsSetting m_graphicsSetting;
};