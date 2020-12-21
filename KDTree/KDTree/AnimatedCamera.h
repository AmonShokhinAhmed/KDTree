#pragma once
#include "Camera.h"
#include <vector>


class AnimatedCamera:public Camera {
public:

    //current velocity of the camera
    float Velocity = 1.0f;
    //bool to decide if the dolly should be looped or not
    bool Loop=false;
    //Kochanek-Bartels parameter, using zero for now to make a Catmull-Rom Spline
    float T = 0;
    float B = 0;
    float C = 0;

    AnimatedCamera();
    ~AnimatedCamera();
    virtual void Update() override;
    

private:
    //variables to store current state
    bool _finished = false;
    bool _firstLoop = true;
    float _curDistance = 0;
    int _prevIndex = 0;
    int _curIndex=0;
    int _nextIndex=1;
    int _nextAfterIndex =1;

    const float distanceToNextPoint();
};
