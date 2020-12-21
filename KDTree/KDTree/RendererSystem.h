#pragma once
#include "System.h"
#include "Renderer.h"
class Renderer;
class RendererSystem : public System<Renderer> {
public:
	virtual void Update() override;
	void RenderScene(const Shader& shader, const bool considerShadowCaster = false);
	virtual void ComponentCheck() override;
	float NormalStrength=0.5f;
};