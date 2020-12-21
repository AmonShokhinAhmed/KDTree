#include "LightSystem.h"


Light* LightSystem::GetFirstLight()
{
	return (*_components.begin());
}

void LightSystem::Update()
{
	for (Light* l : _components) {
		l->Update();
	}
}
