#include "SystemManager.h"


RendererSystem SystemManager::RendererSystem;
LightSystem SystemManager::LightSystem;
CameraSystem SystemManager::CameraSystem;
LineSystem SystemManager::LineSystem;

void SystemManager::Update()
{
	RendererSystem.Update();
	LightSystem.Update();
	LineSystem.Update();
	CameraSystem.Update();
}
