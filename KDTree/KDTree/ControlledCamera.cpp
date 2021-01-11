#include "ControlledCamera.h"
#include "InputManager.h"
#include "Entity.h"

ControlledCamera::ControlledCamera()
{
    SystemManager::CameraSystem.AddComponent(this);
}

ControlledCamera::~ControlledCamera()
{
    SystemManager::CameraSystem.RemoveComponent(this);
}

void ControlledCamera::Update()
{
    float velocity = MovementSpeed * InputManager::DeltaTime();

    if (InputManager::WIsDown())
        _owner->transform.Position += WorldFront*_owner->transform.Rotation * velocity;
    if (InputManager::AIsDown())
        _owner->transform.Position += WorldRight * _owner->transform.Rotation * velocity;
    if (InputManager::SIsDown())
        _owner->transform.Position -= WorldFront * _owner->transform.Rotation * velocity;
    if (InputManager::DIsDown())
        _owner->transform.Position -= WorldRight * _owner->transform.Rotation * velocity;


    _owner->transform.Rotation *= glm::angleAxis(InputManager::DeltaMouseX() * MouseSensitivity, WorldUp);
    _owner->transform.Rotation = glm::angleAxis(InputManager::DeltaMouseY() * MouseSensitivity, WorldRight) * _owner->transform.Rotation;


    Zoom -= InputManager::DeltaScrollY();
    if (Zoom < 1.0f)
        Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

