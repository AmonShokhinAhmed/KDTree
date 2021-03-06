#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Renderer.h"
#include "Entity.h"
#include "SystemManager.h"
#include <glad/glad.h> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <fstream>
#include <sstream>
#include <iostream>

Renderer::Renderer() :
    AmbientColor(0.7f, 0.7f, 0.7f), DiffuseColor(0.7f, 0.7f, 0.7f), SpecularColor(0.1f, 0.1f, 0.1f), Shininess(16.0f),  _shader("default.vert", "default.frag"), Lit(true), CastShadow(true), ReceiveShadow(true)
{
    SystemManager::RendererSystem.AddComponent(this);
}
Renderer::~Renderer()
{
    SystemManager::RendererSystem.RemoveComponent(this);
}
void Renderer::SetModel(string const& path)
{
    loadModel(path);
}

void Renderer::SetTextures(char const* diffuseMapPath, char const* normalMapPath) {
    
    glGenTextures(1, &_diffuseMap);
    glBindTexture(GL_TEXTURE_2D, _diffuseMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;

    unsigned char* diffuseData = stbi_load(diffuseMapPath, &width, &height, &nrChannels, 0);
    if (diffuseData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuseData);
        glGenerateMipmap(GL_TEXTURE_2D);
        _diffuseSet = true;
    }
    else
    {
        std::cout << "Failed to load diffuse texture" << std::endl;
    }
    stbi_image_free(diffuseData);

    glGenTextures(1, &_normalMap);
    glBindTexture(GL_TEXTURE_2D, _normalMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* normalData = stbi_load(normalMapPath, &width, &height, &nrChannels, 0);
    if (normalData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, normalData);
        glGenerateMipmap(GL_TEXTURE_2D);
        _normalSet = true;
    }
    else
    {
        std::cout << "Failed to load normal texture" << std::endl;
    }
    stbi_image_free(normalData);
}

void Renderer::Update()
{
    _shader.use();
    _shader.setVec3("viewPos", SystemManager::CameraSystem.ActiveCamera()->GetOwner()->transform.Position);


    //set light properties, depthmap and planes inside shader
    auto lights = SystemManager::LightSystem.GetLights();
    for (int i = 0; i < lights.size(); i++) {
        _shader.setVec3(("lights["+std::to_string(i)+"].position"), lights[i]->GetOwner()->transform.Position);
        _shader.setVec3("lights[" + std::to_string(i) + "].ambient", lights[i]->AmbientColor);
        _shader.setVec3("lights[" + std::to_string(i) + "].diffuse", lights[i]->DiffuseColor);
        _shader.setVec3("lights[" + std::to_string(i) + "].specular", lights[i]->SpecularColor); 
        _shader.setFloat("lights[" + std::to_string(i) + "].constant", lights[i]->Constant); 
        _shader.setFloat("lights[" + std::to_string(i) + "].linear", lights[i]->Linear);
        _shader.setFloat("lights[" + std::to_string(i) + "].quadratic", lights[i]->Quadratic);
        _shader.setFloat("farPlanes[" + std::to_string(i) + "]", lights[i]->GetFarPlane());
        _shader.setInt("depthMap"+ std::to_string(i), i);
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, lights[i]->GetDepthMap());
    }
    
    //bind textures
    glActiveTexture(GL_TEXTURE0 + lights.size());
    glBindTexture(GL_TEXTURE_2D, _diffuseMap);
    _shader.setInt("diffuseMap", lights.size());
    _shader.setInt("diffuseSet", _diffuseSet);
    glActiveTexture(GL_TEXTURE0 + lights.size()+1);
    glBindTexture(GL_TEXTURE_2D, _normalMap);
    _shader.setInt("normalMap", lights.size() + 1);
    _shader.setInt("normalSet", _normalSet);

    _shader.setFloat("normalStrength", SystemManager::RendererSystem.NormalStrength);


    // material properties
    _shader.setVec3("material.ambient", AmbientColor);
    _shader.setVec3("material.diffuse", DiffuseColor);
    _shader.setVec3("material.specular", SpecularColor);
    _shader.setFloat("material.shininess", Shininess);
    _shader.setInt("material.lit", Lit);
    _shader.setInt("receiveShadows", ReceiveShadow);

   

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(SystemManager::CameraSystem.ActiveCamera()->Zoom), (float)InputManager::SCR_WIDTH / (float)InputManager::SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = SystemManager::CameraSystem.ActiveCamera()->GetViewMatrix();
    _shader.setMat4("projection", projection);
    _shader.setMat4("view", view);

    // world transformation
    Render(_shader);
}

std::vector<glm::vec3> Renderer::GetAllVertices()
{
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, _owner->transform.Position);
    model *= glm::toMat4(_owner->transform.Rotation);
    model = glm::scale(model, _owner->transform.Scale);
    std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
    for (auto const& m : meshes) {
        for (auto const& v : m.vertices) {
            vertices.push_back(glm::vec3(model * glm::vec4(v.Position, 1.0)));
        }
    }
    return vertices;
}

std::vector<unsigned int> Renderer::GetAllIndices()
{
    std::vector<unsigned int> indices = std::vector<unsigned int>();
    unsigned int indicesOffset = 0;
    for (auto const& m : meshes) {
        for (auto const& i : m.indices) {
            indices.push_back(i + indicesOffset);
        }
        indicesOffset += m.vertices.size();
    }
    return indices;
}
//render object
void Renderer::Render(const Shader &shader)
{
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, _owner->transform.Position);
    model *= glm::toMat4(_owner->transform.Rotation);
    model = glm::scale(model, _owner->transform.Scale);
    shader.setMat4("model", model);
    
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw();
    }
}
void Renderer::loadModel(string const& path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}
void Renderer::processNode(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}
Mesh Renderer::processMesh(aiMesh* mesh, const aiScene* scene) {
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            //texture Coordinates
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices);
}
