#include "Mesh.h"
#include <glad/glad.h> 

    Mesh::Mesh() {
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, -1.0f) });

        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f) });
        vertices.push_back({ glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f) });

        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(-1.0f, 0.0f, 0.0f) });

        vertices.push_back({glm::vec3(0, 0, 0),glm::vec3(1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(1.0f, 0.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0),glm::vec3(1.0f, 0.0f, 0.0f) });

        vertices.push_back({glm::vec3(0, 0, 0),  glm::vec3(0.0f, -1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, -1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, -1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, -1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, -1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0),  glm::vec3(0.0f, -1.0f, 0.0f) });

        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0),  glm::vec3(0.0f, 1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0),  glm::vec3(0.0f, 1.0f, 0.0f) });
        vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f) });

        indices = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 };
        setupMesh();
    }

    Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
        setupMesh();
    }


    const void Mesh::Draw()
    {
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void Mesh::setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
