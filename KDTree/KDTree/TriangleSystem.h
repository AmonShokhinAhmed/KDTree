#pragma once
#include "Triangle.h"
#include "System.h"
class TriangleSystem :public System<Triangle>
{
public:
	// Inherited via System
	virtual void Update() override;

	// Inherited via System
	virtual void ComponentCheck() override;
};

