#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Entity.h"

#include <iostream>
#include "Line.h"
#include "Camera.h"
#include "Renderer.h"
#include "CameraSystem.h"
#include "InputManager.h"
#include "Mesh.h"
#include "SystemManager.h"
#include "LightSystem.h"
#include "RendererSystem.h"

GLFWwindow* window;

void sizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(InputManager::SCR_WIDTH, InputManager::SCR_HEIGHT, "Exercise 1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, sizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glfwSetScrollCallback(window, InputManager::ProcessScroll);
}

int main()
{
    int init = initialize();

    if (init != 0) {
        return init;
    }   


    std::vector<Entity*> entities;

    Entity* light = new Entity();
    light->AddComponent<Light>();
    light->transform.Position = glm::vec3(1.2f, 10.0f, 2.0f);
    entities.push_back(light);

    Entity* controlledCamera = new Entity();
    controlledCamera->transform.Position = glm::vec3(-2.0f, 2.0f, 0.0f);
    controlledCamera->AddComponent<Camera>();
    controlledCamera->GetComponent<Camera>()->Active = true;
    entities.push_back(controlledCamera);

    
    Entity* house = new Entity();
    house->AddComponent<Renderer>();
    house->GetComponent<Renderer>()->SetModel("resources/models/house.obj");
    entities.push_back(house);

    Entity* environment = new Entity();
    environment->AddComponent<Renderer>();
    environment->GetComponent<Renderer>()->SetModel("resources/models/environment.obj");
    entities.push_back(environment);
   
    Entity* decor = new Entity();
    decor->AddComponent<Renderer>();
    decor->GetComponent<Renderer>()->SetModel("resources/models/decor.obj");
    entities.push_back(decor);


#pragma region Test
    

    auto vertices = SystemManager::RendererSystem.GetAllVertices();
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    for (auto v : vertices) {
        min.x = min.x > v.x ? v.x : min.x;
        min.y = min.y > v.y ? v.y : min.y;
        min.z = min.z > v.z ? v.z : min.z;

        max.x = max.x < v.x ? v.x : max.x;
        max.y = max.y < v.y ? v.y : max.y;
        max.z = max.z < v.z ? v.z : max.z;
    }
    std::cout << "min: " << min.x << "|" << min.y << "|" << min.z << std::endl;
    std::cout << "max: " << max.x << "|" << max.y << "|" << max.z << std::endl;
    glm::vec3 a = min;
    glm::vec3 b = glm::vec3(max.x,min.y,min.z);
    glm::vec3 c = glm::vec3(max.x, min.y, max.z);
    glm::vec3 d = glm::vec3(min.x,min.y,max.z);
    glm::vec3 e = glm::vec3(min.x,max.y,min.z);
    glm::vec3 f = glm::vec3(max.x,max.y,min.z);
    glm::vec3 g = glm::vec3(max.x,max.y,max.z);
    glm::vec3 h = glm::vec3(min.x,max.y,max.z);



    Entity* ab = new Entity();
    ab->AddComponent<Line>();
    ab->GetComponent<Line>()->SetPoints(a, b);
    entities.push_back(ab);

    Entity* bc = new Entity();
    bc->AddComponent<Line>();
    bc->GetComponent<Line>()->SetPoints(b, c);
    entities.push_back(bc);

    Entity* cd = new Entity();
    cd->AddComponent<Line>();
    cd->GetComponent<Line>()->SetPoints(c, d);
    entities.push_back(cd);

    Entity* da = new Entity();
    da->AddComponent<Line>();
    da->GetComponent<Line>()->SetPoints(d, a);
    entities.push_back(da);


    Entity* ef = new Entity();
    ef->AddComponent<Line>();
    ef->GetComponent<Line>()->SetPoints(e, f);
    entities.push_back(ef);


    Entity* fg = new Entity();
    fg->AddComponent<Line>();
    fg->GetComponent<Line>()->SetPoints(f, g);
    entities.push_back(fg);


    Entity* gh = new Entity();
    gh->AddComponent<Line>();
    gh->GetComponent<Line>()->SetPoints(g, h);
    entities.push_back(gh);


    Entity* he = new Entity();
    he->AddComponent<Line>();
    he->GetComponent<Line>()->SetPoints(h, e);
    entities.push_back(he);

    Entity* ae = new Entity();
    ae->AddComponent<Line>();
    ae->GetComponent<Line>()->SetPoints(a, e);
    entities.push_back(ae);

    Entity* bf = new Entity();
    bf->AddComponent<Line>();
    bf->GetComponent<Line>()->SetPoints(b, f);
    entities.push_back(bf);

    Entity* cg = new Entity();
    cg->AddComponent<Line>();
    cg->GetComponent<Line>()->SetPoints(c, g);
    entities.push_back(cg);

    Entity* dh = new Entity();
    dh->AddComponent<Line>();
    dh->GetComponent<Line>()->SetPoints(d, h);
    entities.push_back(dh);
#pragma endregion


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        InputManager::Update(glfwGetTime(), window);
        SystemManager::Update();

        glfwSetWindowShouldClose(window,InputManager::Escape());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (Entity* e : entities) {
        delete e;
    }
    entities.clear();

    glfwTerminate();
    return 0;
}



