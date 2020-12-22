#include "SystemManager.h"


RendererSystem SystemManager::RendererSystem;
LightSystem SystemManager::LightSystem;
CameraSystem SystemManager::CameraSystem;
LineSystem SystemManager::LineSystem;
TriangleSystem SystemManager::TriangleSystem;

void SystemManager::Update()
{
	TriangleSystem.Update();
	LineSystem.Update();
	RendererSystem.Update();
	LightSystem.Update();
	CameraSystem.Update();
}
