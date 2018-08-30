#include "Light.h"

PointLight::PointLight(
    const Math::Vector3& position,
    const Math::Vector3& color,
    float radius)
	:Light(POINT_LIGHT, color)
{
	m_position = position;
	m_radius = radius;
}

PointLight::~PointLight()
{

}

void PointLight::translate(
    const Math::Vector3& transVector)
{
    m_position += transVector;
}

void PointLight::rotate(
    const Math::Vector3& axis,
    const float angle)
{
    m_position = Math::rotate(m_position, axis, angle);
}
