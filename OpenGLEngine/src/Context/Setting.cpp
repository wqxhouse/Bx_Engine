#include "Setting.h"

unsigned int Setting::getPolyMode()
{
	return polyMode;
}

void Setting::setPolyMode()
{
	polyMode ^= 0;
}
