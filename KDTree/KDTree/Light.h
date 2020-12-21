#pragma once
#include "Component.h"
#include <glm\glm.hpp>
class Light : public Component {
public:
    Light();
    ~Light();
	virtual void Update() override;
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
};