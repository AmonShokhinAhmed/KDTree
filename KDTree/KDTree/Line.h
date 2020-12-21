#pragma once
#include "Component.h"
#include "Shader.h"
class Line : public Component
{
public:
    Line();
    ~Line();
    void SetPoints(glm::vec3 a, glm::vec3 b);
    const void Draw();
    // Inherited via Component
    virtual void Update() override;
private:
    Shader _shader;
    unsigned int _vao;
    unsigned int _vbo;

    float _vertices[6];
};

