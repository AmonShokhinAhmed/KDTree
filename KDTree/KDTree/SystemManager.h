#pragma once
#include "System.h"
#include "LineSystem.h"
#include "TriangleSystem.h"
#include "RendererSystem.h"
#include "LightSystem.h"
#include "CameraSystem.h"
#include "AnimatedCameraPointSystem.h"
#include <iostream>


class SystemManager {
public:
	static TriangleSystem TriangleSystem;
	static LineSystem LineSystem;
	static RendererSystem RendererSystem;
	static LightSystem LightSystem;
	static CameraSystem CameraSystem;
	static AnimatedCameraPointSystem AnimatedCameraPointSystem;
	static void Update();
	static void Reset();
};

