#pragma once
#include "System.h"
#include "RendererSystem.h"
#include "LightSystem.h"
#include "LineSystem.h"
#include "TriangleSystem.h"
#include "CameraSystem.h"
#include <iostream>


class SystemManager {
public:
	static RendererSystem RendererSystem;
	static LightSystem LightSystem;
	static LineSystem LineSystem;
	static TriangleSystem TriangleSystem;
	static CameraSystem CameraSystem;
	static void Update();
};

