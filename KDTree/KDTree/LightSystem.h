#pragma once
#include "Light.h"
#include "System.h"
class LightSystem : public System<Light>
{
public: 
	Light* GetFirstLight();

	virtual void Update() override;
};