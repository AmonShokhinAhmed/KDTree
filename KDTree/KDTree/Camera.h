#pragma once
#include <glm/glm.hpp>
#include "Component.h"

class Camera : public Component
{
public:
    bool Active = false;
    float Zoom = 45.0f;

    // camera options
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.01f;

    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 WorldFront = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 WorldRight = glm::vec3(1.0f, 0.0f, 0.0f);
    Camera();
    ~Camera();
    virtual void Update() override;
    const glm::mat4 GetViewMatrix();
};
