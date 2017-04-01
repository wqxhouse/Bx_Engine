#pragma once

#include "../Core/OpenGLPCH.h"
#include "Setting.h"

class OpenGLContext
{
public:
	OpenGLContext(const Setting& setting);
private:
	Setting setting;
};