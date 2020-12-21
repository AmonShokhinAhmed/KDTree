#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
class Transform
{
public:
	Transform();
	Transform(glm::vec3 position, glm::vec3 scale, glm::quat Rotation);
	Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
	//Construct using single values, with rotation in quaternions
	Transform(float xPosition, float yPosition, float zPosition, float xScale, float yScale, float zScale, float xRotation, float yRotation, float zRotation, float wRotation);
	//Construct using single values, with rotation in euler angles
	Transform(float xPosition, float yPosition, float zPosition, float xScale, float yScale, float zScale, float xRotation, float yRotation, float zRotation);
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::quat Rotation;
};

