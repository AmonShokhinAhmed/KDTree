#include "CameraSystem.h"

Camera* CameraSystem::ActiveCamera()
{
	for (Component* c : _components)
	{
		Camera* cam = ((Camera*)c);
		if (cam->Active)
			return cam;
	}
	return nullptr;
}

void CameraSystem::Update()
{
	for (Camera* c : _components) {
		c->Update();
	}
}
