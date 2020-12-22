
#pragma once
#include "Component.h"
#include "Shader.h"
class Triangle : public Component
{
public:
    glm::vec4 Color = glm::vec4(1.0f, 0.5f, 0.2f, 1.0f);
    Triangle();
    ~Triangle();
    void SetPoints(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    const void Draw();
    // Inherited via Component
    virtual void Update() override;
private:
    Shader _shader;
    unsigned int _vao;
    unsigned int _vbo;

    float _vertices[9];
};

