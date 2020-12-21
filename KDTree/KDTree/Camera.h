#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class Camera : public Component
{
public:
	bool Active = false;
	float Zoom = 45.0f;

	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 WorldFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 WorldRight = glm::vec3(1.0f, 0.0f, 0.0f);

	virtual void Update() = 0;
	const virtual glm::mat4 GetViewMatrix();
protected:
	Camera();
};