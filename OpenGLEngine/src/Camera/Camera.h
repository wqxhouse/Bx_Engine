#pragma once

#include "../Math/Transform/Transform.h"

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float speed);
	virtual void translate(glm::vec3 trans) {}
	virtual void rotate(float degree) {}
	glm::mat4 getViewMat();
protected:
	/*glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;*/
	Transform trans;

	glm::mat4 view;
	float speed;
};

class ProspectiveCamera : public Camera
{
public:
	ProspectiveCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float speed);
	void translate(glm::vec3 translate);
	void rotate(float pitch, float yaw);
};

class OrthographicCamera : public Camera
{

};