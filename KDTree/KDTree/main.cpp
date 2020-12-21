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

    /*
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
    entities.push_back(decor);*/


#pragma region Test
    Entity* line = new Entity();
    line->AddComponent<Line>();
    glm::vec3 a = glm::vec3(-0.5f, 0.5f, 0.0f);
    glm::vec3 b = glm::vec3(0.5f, 0.5f, 0.0f);
    line->GetComponent<Line>()->SetPoints(a,b);
    entities.push_back(line);
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



