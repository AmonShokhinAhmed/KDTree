#include "RendererSystem.h"

void RendererSystem::Update()
{
	glClearColor(0.3f, 0.35f, 0.35f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
