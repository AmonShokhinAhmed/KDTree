#include "TriangleSystem.h"

void TriangleSystem::Update()
{
	for (Triangle* t : _components) {
		t->Update();
	}
}
