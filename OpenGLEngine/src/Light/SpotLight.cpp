#include "Light.h"

SpotLight::SpotLight(
    const Math::Vector3& position,
    const Math::Vector3& color,
    float radius_in,
    float radius_out)
	: Light(SPOT_LIGHT, color)
{
    m_position   = position;
    m_radius_in  = radius_in;
    m_radius_out = radius_out;
}

SpotLight::~SpotLight()
{

}

void SpotLight::translate(Math::Vector3 transVector)
{
}

void SpotLight::rotate(const Math::Vector3 & axis, const float angle)
{
}
