#pragma once

#include "Camera.h"

class ControlledCamera : public Camera
{
public:
    ControlledCamera();
    ~ControlledCamera();
    // camera options
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.01f;
    virtual void Update() override;
};
