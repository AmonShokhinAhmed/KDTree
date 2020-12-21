#pragma once
#include "Component.h"
#include "Shader.h"
#include <glm\glm.hpp>
class Light : public Component {
public:
    Light();
    ~Light();
    glm::mat4 GetLightSpaceMatrix();
    unsigned int GetDepthMap();
    float GetNearPlane();
    float GetFarPlane();
	virtual void Update() override;
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float Constant;
    float Linear;
    float Quadratic;

private:
    unsigned int depthMapFBO;
    unsigned int depthCubeMap;
    glm::mat4 lightSpaceMatrix;
    Shader _depthMapShader;
};