#pragma once
#include "System.h"
#include "Camera.h"

class CameraSystem : public System<Camera>
{
public:
	Camera* ActiveCamera();
	virtual void Update() override;
};