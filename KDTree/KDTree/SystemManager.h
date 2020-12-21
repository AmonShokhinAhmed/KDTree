#pragma once
#include "System.h"
#include "RendererSystem.h"
#include "LightSystem.h"
#include "CameraSystem.h"
#include <iostream>


class SystemManager {
public:
	static RendererSystem RendererSystem;
	static LightSystem LightSystem;
	static CameraSystem CameraSystem;
	static void Update();
};

