#pragma once
#include "Light.h"
#include "System.h"
#define NR_LIGHTS 4
class LightSystem : public System<Light>
{
public: 
	Light* GetLight(unsigned int index);
	std::vector<Light*> GetLights();

	virtual void Update() override;
	virtual void ComponentCheck() override;
};