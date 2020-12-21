#include "Transform.h"

Transform::Transform():
Position(glm::vec3()), Scale(glm::vec3(1,1,1)), Rotation(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Transform::Transform(glm::vec3 position, glm::vec3 scale, glm::quat rotation) :
	Position(position), Scale(scale), Rotation(rotation)
{
}

Transform::Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation):
Position(position), Scale(scale), Rotation(rotation)
{
}

Transform::Transform(float xPosition, float yPosition, float zPosition, float xScale, float yScale, float zScale, float xRotation, float yRotation, float zRotation, float wRotation):
	Position(xPosition,yPosition,zPosition), Scale(xScale,yScale,zScale),Rotation(xRotation,yRotation,zRotation,wRotation)
{
}

Transform::Transform(float xPosition, float yPosition, float zPosition, float xScale, float yScale, float zScale, float xRotation, float yRotation, float zRotation) :
	Position(xPosition, yPosition, zPosition), Scale(xScale, yScale, zScale), Rotation(glm::vec3(xRotation, yRotation, zRotation))
{
}
