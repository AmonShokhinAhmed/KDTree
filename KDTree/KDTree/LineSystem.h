#pragma once
#include "Line.h"
#include "System.h"
class LineSystem:public System<Line>
{
public:
	// Inherited via System
	virtual void Update() override;

	// Inherited via System
	virtual void ComponentCheck() override;
};

