#include "Camera.h"

ProspectiveCamera::ProspectiveCamera(
	glm::vec3 pos, glm::vec3 center, glm::vec3 up, float speed,
	float aspectRatio, float nearClip, float farClip, float fov)
	:Camera(pos, center, up, speed)
{
	//this->view = glm::lookAt(pos, center, up);
	this->proj = glm::perspective(fov, aspectRatio, nearClip, farClip);
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	//this->speed = speed;
}

void ProspectiveCamera::translate(glm::vec3 translate)
{
	trans.pos += translate * speed;
	view = trans.getViewMat();
}

void ProspectiveCamera::rotate(float pitch, float yaw)
{
	//pitch += pitch_offset * CAMERA_SENSATIVE, yaw += yaw_offset * CAMERA_SENSATIVE;
	CLAMP(pitch, -89.0f, 89.0f);

	glm::mat4 rotationX;
	glm::mat4 rotationY;
	rotationY = glm::rotate(rotationY, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 rotationXFront = glm::normalize(rotationY * glm::vec4(curFront, 0.0f));
	glm::vec3 rotationAxis = glm::vec3(-rotationXFront.z, 0.0f, rotationXFront.x);

	rotationX = glm::rotate(rotationX, glm::radians(pitch), rotationAxis);
	trans.front = glm::normalize(rotationX * glm::vec4(rotationXFront, 0.0f));
	
	//TODO: Fix rotation bug
	/*glm::vec3 direction;
	direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));
	direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	trans.front = glm::normalize(direction);*/
	/*printf("%f %f, %f, %f\n", pitch, trans.front.x, trans.front.y, trans.front.z);
	printf("---------------------------------------------------------------------------------\n");
	printf("%f, %f, %f, %f\n", rotationX[0][0], rotationX[0][1], rotationX[0][2], rotationX[0][3]);
	printf("%f, %f, %f, %f\n", rotationX[1][0], rotationX[1][1], rotationX[1][2], rotationX[1][3]);
	printf("%f, %f, %f, %f\n", rotationX[2][0], rotationX[2][1], rotationX[2][2], rotationX[2][3]);
	printf("%f, %f, %f, %f\n", rotationX[3][0], rotationX[3][1], rotationX[3][2], rotationX[3][3]);
	printf("=================================================================================\n");*/
	trans.right = glm::normalize(glm::cross(trans.front, worldUp));
	//trans.right = glm::vec3(sin(glm::radians(yaw) - HALF_PI), 0, cos(glm::radians(yaw) - HALF_PI));
	trans.up = glm::normalize(glm::cross(trans.right, trans.front));
	view = trans.getViewMat();
}

void ProspectiveCamera::update(float deltaTime)
{
	rotate(static_cast<float>(-callbackInfo.cursorPosCallBack.delta_y) * CAMERA_SENSATIVE, 
		   static_cast<float>( callbackInfo.cursorPosCallBack.delta_x) * CAMERA_SENSATIVE);

	Camera::update(deltaTime);
}

glm::mat4 ProspectiveCamera::getProjectionMatrix()
{
	return proj;
}
