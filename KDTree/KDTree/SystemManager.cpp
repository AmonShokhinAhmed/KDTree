#include "SystemManager.h"


RendererSystem SystemManager::RendererSystem;
LightSystem SystemManager::LightSystem;
CameraSystem SystemManager::CameraSystem;
AnimatedCameraPointSystem SystemManager::AnimatedCameraPointSystem;

void SystemManager::Update()
{
	LightSystem.Update();
	AnimatedCameraPointSystem.Update();
	CameraSystem.Update();
	RendererSystem.Update();
}
void SystemManager::Reset()
{
	LightSystem.Reset();
	AnimatedCameraPointSystem.Reset();
	CameraSystem.Reset();
	RendererSystem.Reset();
}