#pragma once
#include <glm/glm.hpp>
#include <vector>

using namespace std;

struct Vertex {

    glm::vec3 Position;

    glm::vec3 Normal;

    glm::vec2 TexCoords;

    glm::vec3 Tangent;

    glm::vec3 Bitangent;
};

class Mesh {
public:

    vector<Vertex>       vertices;
    vector<unsigned int> indices;
	unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

    const void Draw();

private:

    unsigned int VBO, EBO;

    void setupMesh();
};