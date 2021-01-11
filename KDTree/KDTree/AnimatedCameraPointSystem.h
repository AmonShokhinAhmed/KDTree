#pragma once
#include "System.h"
#include "AnimatedCameraPoint.h"
class AnimatedCameraPointSystem :public System <AnimatedCameraPoint> {
public:
	virtual void Update() override;
	virtual void ComponentCheck() override;
};