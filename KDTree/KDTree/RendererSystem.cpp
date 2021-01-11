#include "RendererSystem.h"

void RendererSystem::Update()
{
	for (Renderer* r : _components) {
		r->Update();
	}
}

void RendererSystem::ComponentCheck()
{
}

void RendererSystem::RenderScene(const Shader& shader, const bool considerShadowCaster)
{
	//considerShadowCaster is mostly used when drawing from lights, because normal render operation should generally consider everything
	if (!considerShadowCaster) {
		for (Renderer* r : _components) {
			r->Render(shader);
		}
	}
	else {
		for (Renderer* r : _components) {
			if (r->CastShadow) {
				r->Render(shader);
			}
		}
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
			indices.push_back(i + indicesOffset);
		}
		indicesOffset += r->GetAllVertices().size();
	}
	return indices;
}