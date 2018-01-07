#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 direction, glm::vec3 color)
{
	this->position = position;
	this->direction = direction;
	this->color = color;
}

Light::~Light()
{
}

glm::vec3 Light::getPos()
{
	return position;
}

glm::vec3 Light::getDir()
{
	return direction;
}