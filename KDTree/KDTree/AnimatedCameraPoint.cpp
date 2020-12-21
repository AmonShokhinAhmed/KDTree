#include "AnimatedCameraPoint.h"
#include "SystemManager.h"

AnimatedCameraPoint::AnimatedCameraPoint()
{
	SystemManager::AnimatedCameraPointSystem.AddComponent(this);
}

AnimatedCameraPoint::~AnimatedCameraPoint()
{
	SystemManager::AnimatedCameraPointSystem.RemoveComponent(this);
}

void AnimatedCameraPoint::Update()
{
}
