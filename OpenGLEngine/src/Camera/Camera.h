#pragma once

#include "../Math/Transform/Transform.h"

#define CAMERA_SENSATIVE 0.05f
#define HALF_PI 1.57079632f

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 center, glm::vec3 up, float speed);
	virtual void translate(glm::vec3 trans) {}
	virtual void rotate(float degree) {}

	glm::mat4 getViewMatrix();
	Transform getTrans();

	//void updateKeyboardMotion(CallbackInfo* key, bool motionValue);
	//void updateMouseMotion(double delta_x, double delta_y);
	virtual void update(float deltaTime);
	virtual void draw() {}
protected:
	Transform trans;

	glm::mat4 view;
	float speed;

	//CallbackInfo* callbackInfo;
	//int translationKey[1024];
	//float delta_x;
	//float delta_y;
	glm::vec3 worldUp;

	glm::vec3 curFront;
	glm::vec3 curRight;
};

class ProspectiveCamera : public Camera
{
public:
	ProspectiveCamera(
		glm::vec3 pos, glm::vec3 center, glm::vec3 up, float speed,
		float aspectRatio, float nearClip = 0.1f, float farClip = 100.0f, float fov = 45.0f);
	void translate(glm::vec3 translate);
	void rotate(float pitch, float yaw);

	void update(float deltaTime);

	glm::mat4 getProjectionMatrix();
private:
	float fov;
	float nearClip;
	float farClip;

	glm::mat4 proj;
};

class OrthographicCamera : public Camera
{

};