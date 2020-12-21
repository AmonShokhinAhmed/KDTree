#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Entity.h"

#include <iostream>
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
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";



    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
#pragma endregion


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.35f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        InputManager::Update(glfwGetTime(), window);
        SystemManager::Update();


        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); 
        glDrawArrays(GL_LINE_STRIP, 0, 2);

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



