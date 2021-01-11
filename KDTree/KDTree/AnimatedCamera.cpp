#include "AnimatedCamera.h"

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Entity.h"
#include "AnimatedCameraPoint.h"
#include "InputManager.h"
#include "AnimatedCameraPointSystem.h"
#include "SystemManager.h"

AnimatedCamera::AnimatedCamera()
{
    SystemManager::CameraSystem.AddComponent(this);
}

AnimatedCamera::~AnimatedCamera()
{
    SystemManager::CameraSystem.RemoveComponent(this);
}

void AnimatedCamera::Update() {
    
    //getting the waypoints for the dolly
    auto points = SystemManager::AnimatedCameraPointSystem.GetComponentList();

    //if dolly is finished, the camera is not active or there's no waypoints do nothing, 
    if (_finished||!Active||points.size()<1) {
        return;
    }

    //changing the velocity through key board inputs
    if (InputManager::Q()) {
        Velocity -= 1.0f;
        if (Velocity <= 0) {
            Velocity += 1.0f;
        }
    }
    if (InputManager::E()) {
        Velocity += 1.0f;
    }

    // if there's only one point stay at that point
    if (points.size() == 1) {
        _owner->transform.Position = points.at(0)->GetOwner()->transform.Position;
        _owner->transform.Rotation = points.at(0)->GetOwner()->transform.Rotation;
        return;
    }


    //distance the camera travels during this frame
    float distance = Velocity * InputManager::DeltaTime();
    
    //calculating the distance the camera already traveled between the currently relevant points
    _curDistance += distance;
    
    //t is calculated using the max distance the camera has to travel and the distance it already has travelled
    float t = _curDistance / distanceToNextPoint();
    
    //if t is bigger than 1 we reached the next point and have to set and reset the relevant variables
    if (t > 1) {
        //setting the current distance to the correct value
        _curDistance -= distanceToNextPoint();

        //incrementing the indices
        _prevIndex = _curIndex;
        _curIndex=_nextIndex;

        //checking if we reached the last waypoint
        if (_curIndex >= points.size()-1) {

            //if we want to loop the next index should be the start of the loop
            if (Loop) {
                _nextIndex = 0;
            }
            //else we stop moving
            else {
                _finished = true;
                return;
            }
        }
        //if we didnt reach the end we just increment normally
        else {
            _nextIndex++; 
        }
        //recalculating t
        t = _curDistance / distanceToNextPoint();
    }
    //incrementing the index following the next index, because it is needed for squad
    if (_nextIndex == points.size()-1) {
        if (Loop) {
            _nextAfterIndex = 0;
        }
        else {
            _nextAfterIndex = _nextIndex;
        }
    }
    else {
        _nextAfterIndex = _nextIndex+1;
    }


    //getting the position of the waypoints
    glm::vec3 pMinus1 = points.at(_prevIndex)->GetOwner()->transform.Position;
    glm::vec3 p0 = points.at(_curIndex)->GetOwner()->transform.Position;
    glm::vec3 p1 = points.at(_nextIndex)->GetOwner()->transform.Position;
    glm::vec3 p2 = points.at(_nextAfterIndex)->GetOwner()->transform.Position;

    //calculating the tangent using Kochanek-Bartels
    glm::vec3 m0 = (((1 - T) * (1 + B) * (1 + C)) * 0.5f) * (p0 - pMinus1) + (((1 - T) * (1 - B) * (1 - C)) * 0.5f) * (p1 - p0);
    glm::vec3 m1 = (((1 - T) * (1 + B) * (1 - C)) * 0.5f) * (p1 - p0) + (((1 - T) * (1 - B) * (1 + C)) * 0.5f) * (p2 - p1);

    //using the tangents to calculate the current point with a cubic hermit spline curve
    glm::vec3 p = (2 * pow(t, 3) - 3 * pow(t, 2) + 1) * p0 + (pow(t, 3) - 2 * pow(t, 2) + t) * m0 + (-2 * pow(t, 3) + 3 * pow(t, 2)) * p1 + (pow(t, 3) - pow(t, 2)) * m1;


    //setting the position of the camera
    _owner->transform.Position = p;


    //getting the rotation of the waypoints
    glm::quat rMinus1 = points.at(_prevIndex)->GetOwner()->transform.Rotation;
    glm::quat r0 = points.at(_curIndex)->GetOwner()->transform.Rotation;
    glm::quat r1 = points.at(_nextIndex)->GetOwner()->transform.Rotation;
    glm::quat r2 = points.at(_nextAfterIndex)->GetOwner()->transform.Rotation;

    //calculating intermediate quaternions because they are needed for the squad calculation
    glm::quat s1 = glm::intermediate(rMinus1, r0, r1);
    glm::quat s2 = glm::intermediate(r0, r1, r2);

    //using squad to calculate the current rotation of the camera
    _owner->transform.Rotation = glm::squad(r0, r1, s1,s2, t);


    //uncomment next to lines to use slerp instead of squad
    //_owner->transform.Rotation = glm::slerp(r0, r1, t);
    
}


//calculating the maximal distance between two waypoints
//not very accurate but suffices for now
const float AnimatedCamera::distanceToNextPoint()
{
    auto points = SystemManager::AnimatedCameraPointSystem.GetComponentList();
    return glm::distance(points.at(_curIndex)->GetOwner()->transform.Position, points.at(_nextIndex)->GetOwner()->transform.Position);
}
