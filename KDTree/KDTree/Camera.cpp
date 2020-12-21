#include "Camera.h"
#include "InputManager.h"
#include "Entity.h"

Camera::Camera()
{
    SystemManager::CameraSystem.AddComponent(this);
}

Camera::~Camera()
{
    SystemManager::CameraSystem.RemoveComponent(this);
}

void Camera::Update()
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

const glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(_owner->transform.Position, _owner->transform.Position + WorldFront * _owner->transform.Rotation, WorldUp * _owner->transform.Rotation);
}