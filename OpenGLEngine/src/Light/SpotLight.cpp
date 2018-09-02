#include "Light.h"

SpotLight::SpotLight(
    const Math::Vector3& position,
    const Math::Vector3& direction,
    const Math::Vector3& color,
    float distance,
    float in_angle,
    float out_angle)
	: Light(SPOT_LIGHT, color),
      m_position(position),
      m_direction(direction)
{
    m_in_cosTheta  = std::cos(in_angle);
    m_out_cosTheta = std::cos(out_angle);

    reserve = distance; // Use the reserve to store distance
}

SpotLight::~SpotLight()
{
}

void SpotLight::translate(
    const Math::Vector3& transVector)
{
    m_position += transVector;
}

void SpotLight::rotate(
    const Math::Vector3 & axis,
    const float angle)
{
    m_direction = Math::rotate(m_direction, axis, angle);
}
