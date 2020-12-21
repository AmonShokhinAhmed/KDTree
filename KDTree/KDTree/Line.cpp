#include "Entity.h"
#include "Line.h"
#include <glad\glad.h>
#include <iostream>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

Line::Line() :
_shader("line.vert", "line.frag") {
    SystemManager::LineSystem.AddComponent(this);
}
void Line::SetPoints(glm::vec3 a, glm::vec3 b) {
    _vertices[0] = a.x;
    _vertices[1] = a.y;
    _vertices[2] = a.z;
    _vertices[3] = b.x;
    _vertices[4] = b.y;
    _vertices[5] = b.z;
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}
const void Line::Draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_LINE_STRIP, 0, 2);
    glBindVertexArray(0);
}

void Line::Update()
{
    _shader.use(); 
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(SystemManager::CameraSystem.ActiveCamera()->Zoom), (float)InputManager::SCR_WIDTH / (float)InputManager::SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = SystemManager::CameraSystem.ActiveCamera()->GetViewMatrix();
    _shader.setMat4("projection", projection);
    _shader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, _owner->transform.Position);
    model *= glm::toMat4(_owner->transform.Rotation);
    model = glm::scale(model, _owner->transform.Scale);
    _shader.setMat4("model", model);
    Draw();
}
