#include "SystemManager.h"


RendererSystem SystemManager::RendererSystem;
LightSystem SystemManager::LightSystem;
CameraSystem SystemManager::CameraSystem;

void SystemManager::Update()
{
	RendererSystem.Update();
	LightSystem.Update();
	CameraSystem.Update();
}
