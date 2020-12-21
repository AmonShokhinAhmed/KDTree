#include "Light.h"
#include "SystemManager.h"

Light::Light() :
	AmbientColor(0.6f, 0.6f,0.6f), DiffuseColor(0.7f, 0.7, 0.7f), SpecularColor(0.5, 0.5, 0.5)
{
	SystemManager::LightSystem.AddComponent(this);
}

Light::~Light()
{
	SystemManager::LightSystem.RemoveComponent(this);
}

void Light::Update()
{
}
