#include "SystemManager.h"


RendererSystem SystemManager::RendererSystem;
LineSystem SystemManager::LineSystem;
TriangleSystem SystemManager::TriangleSystem;
LightSystem SystemManager::LightSystem;
CameraSystem SystemManager::CameraSystem;
AnimatedCameraPointSystem SystemManager::AnimatedCameraPointSystem;

void SystemManager::Update()
{
	LightSystem.Update();
	AnimatedCameraPointSystem.Update();
	LineSystem.Update();
	TriangleSystem.Update();
	RendererSystem.Update();
	CameraSystem.Update();
}
void SystemManager::Reset()
{
	LightSystem.Reset();
	AnimatedCameraPointSystem.Reset();
	CameraSystem.Reset();
	TriangleSystem.Reset();
	LineSystem.Reset();
	RendererSystem.Reset();
}