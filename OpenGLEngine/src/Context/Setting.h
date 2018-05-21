#pragma once

enum PolyMode
{
	WIREFRAME = 0x00000000,
	TRIANGLE = 0x00000001,
};

struct GraphicsSetting
{
public:
	enum Antialasing
	{
		NONE	=	0x00000000,
		LOW		=	0x00000002,
		HIGH	=	0x00000004,
		ELTRA	=	0x00000008,
	}antialasing = Antialasing::HIGH;
};

struct Setting
{
public:
	UINT width = 1280;
    UINT height = 720;

	float fov = 45.0f;

	PolyMode polyMode = PolyMode::TRIANGLE;

	GraphicsSetting m_graphicsSetting;
};