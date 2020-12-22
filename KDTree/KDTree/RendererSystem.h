#pragma once
#include "System.h"
#include "Renderer.h"
class Renderer;
class RendererSystem : public System<Renderer> {
public:
	virtual void Update() override;
	std::vector<glm::vec3> GetAllVertices();
	std::vector<unsigned int> GetAllIndices();
};