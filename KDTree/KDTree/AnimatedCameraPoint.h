#pragma once
#include "Component.h"

class AnimatedCameraPoint :public Component {
public:
	AnimatedCameraPoint();
	~AnimatedCameraPoint();
	virtual void Update() override;
};