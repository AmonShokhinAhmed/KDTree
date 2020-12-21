#pragma once
#include "System.h"
#include "Renderer.h"
class Renderer;
class RendererSystem : public System<Renderer> {
public:
	virtual void Update() override;
};