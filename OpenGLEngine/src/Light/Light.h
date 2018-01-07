#pragma once

#include "../Core/OpenGLPCH.h"

class Light
{
public:
	Light(glm::vec3 position, glm::vec3 direction, glm::vec3 color);
	~Light();

	glm::vec3 getPos();
	glm::vec3 getDir();
protected:
	glm::vec3 position; // Light position, set to (0,0,0) for directional light
	glm::vec3 direction; // Light direction, set to (0,0,0) for point light
	glm::vec3 color; // Light color
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color);
	~DirectionalLight();
};

class PointLight : public Light
{
public:
};

class SpotLight : public Light
{
public:
};

//TODO: other kinds of lights