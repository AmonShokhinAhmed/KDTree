#include "LineSystem.h"

void LineSystem::Update()
{
	for (Line* l : _components) {
		l->Update();
	}
}
