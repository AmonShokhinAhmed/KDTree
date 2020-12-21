#include "RendererSystem.h"

void RendererSystem::Update()
{
	for (Renderer* r : _components) {
		r->Update();
	}
}
