#pragma once

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <string>
#include <vector>

#include "InputManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "Component.h"

using namespace std;

class Renderer : public Component
{
public:

    vector<Mesh>    meshes;
    string directory;
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float Shininess;
    bool Lit;
    bool CastShadow;
    bool ReceiveShadow;

    Renderer();
    ~Renderer();

    void SetModel(string const& path); 

    void SetTextures(char const* diffuseMapPath, char const* normalMapPath);

    virtual void Update() override;
    void Render(const Shader& shader);

private:
    
    Shader _shader;

    bool _diffuseSet = false;
    unsigned int _diffuseMap;
    bool _normalSet = false;
    unsigned int _normalMap;
    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};