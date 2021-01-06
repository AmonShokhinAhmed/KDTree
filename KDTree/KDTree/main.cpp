#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "KDTree.h"
#include "Shader.h"
#include "Entity.h"

#include <iostream>
#include "Triangle.h"
#include "Line.h"
#include "Camera.h"
#include "Renderer.h"
#include "CameraSystem.h"
#include "InputManager.h"
#include "Mesh.h"
#include "SystemManager.h"
#include "LightSystem.h"
#include "RendererSystem.h"
#include <chrono>

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

	Entity* camera = new Entity();
	camera->transform.Position = glm::vec3(-2.0f, 2.0f, 0.0f);
	camera->AddComponent<Camera>();
	camera->GetComponent<Camera>()->Active = true;
	entities.push_back(camera);





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

	Entity* floor = new Entity();
	floor->AddComponent<Renderer>();
	floor->GetComponent<Renderer>()->SetModel("resources/models/floor.obj");
	entities.push_back(floor);


	//Generate Random Boxes
	/*
	float lowerBound = -1;
	float upperBound = 1;
	for (int i = 0; i < 15; i++) {
		Entity* cube = new Entity();
		cube->AddComponent<Renderer>();
		cube->GetComponent<Renderer>()->SetModel("resources/models/cube.obj");
		float x = lowerBound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperBound - lowerBound)));
		float y = lowerBound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperBound - lowerBound)));
		float z = lowerBound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperBound - lowerBound)));

		cube->transform.Scale = glm::vec3(0.1f, 0.1f, 0.1f);
		cube->transform.Position = glm::vec3(x, y, z);
		entities.push_back(cube);
	}*/
	//Test vertices
	/*std::vector<glm::vec3> vertices{
		glm::vec3(0,3,1),
		glm::vec3(1,9,1),
		glm::vec3(2,0,3),
		glm::vec3(3,6,4),
		glm::vec3(4,4,5),
		glm::vec3(7,5,5),
		glm::vec3(0,6,0),
		glm::vec3(5,9,7),
		glm::vec3(8,3,2),
		glm::vec3(2,3,9),
	};*/
	//Random Spheres
	/*float lowerBound = -1;
	float upperBound = 1;
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < 15; i++) {
		float x = lowerBound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperBound - lowerBound)));
		float y = lowerBound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperBound - lowerBound)));
		float z = lowerBound + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperBound - lowerBound)));

		vertices.push_back(glm::vec3(x, y, z));
	}
	for (auto v : vertices) {
		Entity* sphere = new Entity();
		sphere->AddComponent<Renderer>();
		sphere->GetComponent<Renderer>()->SetModel("resources/models/sphere.obj");

		sphere->transform.Scale = glm::vec3(0.05f, 0.05f, 0.05f);
		sphere->transform.Position =v;
		entities.push_back(sphere);
	}*/

	auto vertices = SystemManager::RendererSystem.GetAllVertices();
	auto indices = SystemManager::RendererSystem.GetAllIndices();
	int approxChildTris = 150;
	KDTree kd = KDTree(vertices, indices, approxChildTris);



	/*auto minTriY = kd.FindMinTriangle(2);
	std::cout << "Triangle found: " << vec3ToString(minTriY.a)<<" , " << vec3ToString(minTriY.b)<<" , " << vec3ToString(minTriY.c)<<" , " << std::endl;
	Entity* tMinY = new Entity();
	tMinY->AddComponent<Triangle>();
	tMinY->GetComponent<Triangle>()->SetPoints(minTriY.a, minTriY.b, minTriY.c);
	entities.push_back(tMinY);
	*/

	Entity* rayCastLine = nullptr;
	Entity* rayCastTriangle = nullptr;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.3f, 0.35f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		InputManager::Update(glfwGetTime(), window);
		SystemManager::Update();

		if (InputManager::RMBIsDown()) {
			Ray ray;
			ray.A = camera->transform.Position;
			ray.D = camera->GetComponent<Camera>()->WorldFront * camera->transform.Rotation;
			ray.TMax = 10;

			if (rayCastLine == nullptr) {
				rayCastLine = new Entity();
				rayCastLine->AddComponent<Line>();
				entities.push_back(rayCastLine);
			}
			glm::vec3 a = ray.A;
			glm::vec3 b = ray.A + ray.D * ray.TMax;
			rayCastLine->GetComponent<Line>()->SetPoints(a, b);
			rayCastLine->GetComponent<Line>()->Color = glm::vec4(0.0f, 1.0f, 0.2f, 1.0f);

			auto start = std::chrono::high_resolution_clock::now();
			Hit* hit = kd.RayIntersect(ray);
			auto end = std::chrono::high_resolution_clock::now();
			std::cout << "Raycast took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
			if (hit != nullptr) {
				if (rayCastTriangle == nullptr) {
					rayCastTriangle = new Entity();
					rayCastTriangle->AddComponent<Triangle>(); 
					entities.push_back(rayCastTriangle);
				}
				rayCastTriangle->GetComponent<Triangle>()->SetPoints(hit->Triangle->a, hit->Triangle->b, hit->Triangle->c);
			}
			else {
				std::cout << "nothing hit" << std::endl;
			}
		}

		glfwSetWindowShouldClose(window, InputManager::Escape());
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



