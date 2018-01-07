#pragma once

struct GraphicsSetting
{
public:
	enum Antialasing
	{
		NONE = 0, LOW = 2, HIGH = 4, ELTRA = 8
	}antialasing = Antialasing::HIGH;
};

struct Setting
{
public:
	unsigned int width = 1280;
	unsigned int height = 720;

	float fov = 45.0f;

	GraphicsSetting m_graphicsSetting;

	unsigned int getPolyMode();
	void setPolyMode();
private:
	unsigned int polyMode = 0;
};