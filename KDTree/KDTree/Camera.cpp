#include "Camera.h"
#include "Entity.h"
#include <glm\ext\matrix_transform.hpp>

const glm::mat4 Camera::GetViewMatrix()
{
        return glm::lookAt(_owner->transform.Position, _owner->transform.Position + WorldFront * _owner->transform.Rotation, WorldUp * _owner->transform.Rotation);
}

Camera::Camera()
{
}
