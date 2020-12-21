#include "InputManager.h"
#include "LightSystem.h"
#include <glad\glad.h>

Light* LightSystem::GetLight(unsigned int index)
{
	if (index >= _components.size()) {
		return *_components.end();
	}
	return (_components.at(index));
}
std::vector<Light*> LightSystem::GetLights() {
	return _components;
}


void LightSystem::Update()
{
	//Clear SCreen
	glClearColor(0.3f, 0.35f, 0.35f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Update every light
	for (Light* l : _components) {
		l->Update();
	}
	//reset viewport
	glViewport(0, 0, InputManager::SCR_WIDTH, InputManager::SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LightSystem::ComponentCheck()
{
	if (_components.size() > NR_LIGHTS) {
		_components.pop_back();
	}
}




