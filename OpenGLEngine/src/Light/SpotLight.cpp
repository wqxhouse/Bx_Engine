#include "Light.h"

SpotLight::SpotLight(const glm::vec3 position, const glm::vec3& color, float radius_in, float raidus_out)
	:Light(SPOT_LIGHT, color)
{
	m_position   = position;
	m_radius_in  = radius_in;
	m_radius_out = m_radius_out;
}

SpotLight::~SpotLight()
{

}