#pragma once
#include "System.h"
#include "Renderer.h"
class Renderer;
class RendererSystem : public System<Renderer> {
public:
	virtual void Update() override;
	void RenderScene(const Shader& shader, const bool considerShadowCaster = false);
	virtual void ComponentCheck() override;

	std::vector<glm::vec3> GetAllVertices();
	std::vector<unsigned int> GetAllIndices();
	float NormalStrength = 0.5f;
};