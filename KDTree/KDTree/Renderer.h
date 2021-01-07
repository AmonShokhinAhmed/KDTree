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
    bool Unlit;

    Renderer();
    ~Renderer();

    void SetModel(string const& path); 
    void SetShader(const char* vertPath, const char* fragPath, const char* geoPath);
    virtual void Update() override;
    std::vector<glm::vec3> GetAllVertices();
    std::vector<unsigned int> GetAllIndices();

private:
    
    Shader _shader;
    
    void loadModel();
    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};