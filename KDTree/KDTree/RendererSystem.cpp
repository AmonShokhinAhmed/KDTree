#include "RendererSystem.h"

void RendererSystem::Update()
{
	for (Renderer* r : _components) {
		r->Update();
	}
}

std::vector<glm::vec3> RendererSystem::GetAllVertices()
{
	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
	for (Renderer* r : _components) {
		std::vector<glm::vec3> newVertices = r->GetAllVertices();
		vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
	}
	return vertices;
}

std::vector<unsigned int> RendererSystem::GetAllIndices()
{
	std::vector<unsigned int> indices = std::vector<unsigned int>();
	unsigned int indicesOffset = 0;
	for (Renderer* r : _components) {
		std::vector<unsigned int> newIndices = r->GetAllIndices();
		for (auto i : newIndices) {
			indices.push_back(i+indicesOffset);
		}
		indicesOffset += r->GetAllVertices().size();
	}
	return indices;
}
