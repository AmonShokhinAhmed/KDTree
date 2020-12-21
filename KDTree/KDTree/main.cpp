#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Entity.h"

#include <iostream>
#include "ControlledCamera.h"
#include "AnimatedCamera.h"
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

int initialize(int samples) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_SAMPLES, samples);

	window = glfwCreateWindow(InputManager::SCR_WIDTH, InputManager::SCR_HEIGHT, "KD Tree", NULL, NULL);
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	glfwSetScrollCallback(window, InputManager::ProcessScroll);
}

int main()
{
	bool restart = false;
	int samples = 4;
	bool aaEnabled = true;
	do {
		int init = initialize(samples);
		if (init != 0) {
			return init;
		}

		std::vector<Entity*> entities;

		glm::vec3 lightPositions[] = {
			glm::vec3(-4.0f, 4.0f, -4.0f),
			glm::vec3(-4.0f, 4.0f, 4.0f),
			glm::vec3(4.0f, 4.0f, -4.0f),
			glm::vec3(4.0f, 4.0f, 4.0f) };

		for (int i = 0; i < NR_LIGHTS; i++) {

			Entity* light = new Entity();
			light->AddComponent<Light>();
			light->transform.Position = lightPositions[i];
			light->transform.Rotation *= glm::angleAxis(195.0f, glm::vec3(1.0f, 0.0f, 0.0f)) * light->transform.Rotation;
			light->transform.Scale *= glm::vec3(0.1f, 0.1f, 0.1f);
			light->AddComponent<Renderer>();
			light->GetComponent<Renderer>()->SetModel("resources/models/sphere.obj");
			light->GetComponent<Renderer>()->CastShadow = false;
			light->GetComponent<Renderer>()->ReceiveShadow = false;
			light->GetComponent<Renderer>()->DiffuseColor = glm::vec3(1, 1, 1);
			light->GetComponent<Renderer>()->Lit = false;
			entities.push_back(light);
		}
		Entity* controlledCamera = new Entity();
		controlledCamera->transform.Position = glm::vec3(-2.0f, 2.0f, 0.0f);
		controlledCamera->AddComponent<ControlledCamera>();
		controlledCamera->GetComponent<ControlledCamera>()->Active = true;
		entities.push_back(controlledCamera);

		Entity* animatedCamera = new Entity();
		animatedCamera->AddComponent<AnimatedCamera>();
		animatedCamera->GetComponent<AnimatedCamera>()->Active = false;
		animatedCamera->GetComponent<AnimatedCamera>()->Loop = true;
		entities.push_back(animatedCamera);

		/*Entity* bread1 = new Entity();
		bread1->AddComponent<Renderer>();
		bread1->GetComponent<Renderer>()->SetModel("resources/models/bread1.obj");
		bread1->GetComponent<Renderer>()->SetTextures("resources/textures/bread1diffuse.jpg", "resources/textures/bread1normal.jpg");
		entities.push_back(bread1);
		bread1->transform.Scale = glm::vec3(30.0f, 30.0f, 30.0f);
		bread1->transform.Position = glm::vec3(10.0f, 0.0f, 5.0f);*/

		/*Entity* bread2 = new Entity();
		bread2->AddComponent<Renderer>();
		bread2->GetComponent<Renderer>()->SetModel("resources/models/bread2.obj");
		bread2->GetComponent<Renderer>()->SetTextures("resources/textures/bread2diffuse.jpg", "resources/textures/bread2normal.jpg");
		entities.push_back(bread2);
		bread2->transform.Scale = glm::vec3(30.0f, 30.0f, 30.0f);
		bread2->transform.Position = glm::vec3(-10.0f, 0.0f, 5.0f);*/

		Entity* bread3 = new Entity();
		bread3->AddComponent<Renderer>();
		bread3->GetComponent<Renderer>()->SetModel("resources/models/bread3.obj");
		bread3->GetComponent<Renderer>()->SetTextures("resources/textures/bread3diffuse.jpg", "resources/textures/bread3normal.jpg");
		entities.push_back(bread3);
		bread3->transform.Scale = glm::vec3(30.0f, 30.0f, 30.0f);
		bread3->transform.Position = glm::vec3(0.0f, 0.0f, 5.0f);

		Entity* floor = new Entity();
		floor->AddComponent<Renderer>();
		floor->GetComponent<Renderer>()->SetModel("resources/models/floor.obj");
		floor->GetComponent<Renderer>()->SetTextures("resources/textures/bricksdiffuse.jpg", "resources/textures/bricksnormal.jpg");
		entities.push_back(floor);


		glfwSetWindowShouldClose(window, false);
		restart = false;
		while (!glfwWindowShouldClose(window)&&!restart)
		{
			InputManager::Update(glfwGetTime(), window);
			SystemManager::Update();

			//setting new waypoints
			if (InputManager::Space() && controlledCamera->GetComponent<ControlledCamera>()->Active) {

				Entity* point = new Entity();
				point->transform.Position = glm::vec3(controlledCamera->transform.Position);
				point->transform.Scale = glm::vec3(0.2f, 0.2f, 0.2f);
				point->transform.Rotation = glm::quat(controlledCamera->transform.Rotation);
				point->AddComponent<AnimatedCameraPoint>();


				point->AddComponent<Renderer>();
				point->GetComponent<Renderer>()->SetModel("resources/models/sphere.obj");
				point->GetComponent<Renderer>()->AmbientColor = glm::vec3(1, 0, 0);
				point->GetComponent<Renderer>()->DiffuseColor = glm::vec3(1, 0, 0);
				point->GetComponent<Renderer>()->SpecularColor = glm::vec3(1, 0, 0);
				entities.push_back(point);
			}

			if (InputManager::MIsDown()) {
				SystemManager::RendererSystem.NormalStrength += 0.01;
				if (SystemManager::RendererSystem.NormalStrength > 2) {
					SystemManager::RendererSystem.NormalStrength = 2;
				}
			}
			if (InputManager::NIsDown()) {
				SystemManager::RendererSystem.NormalStrength -= 0.01;
				if (SystemManager::RendererSystem.NormalStrength < 0) {
					SystemManager::RendererSystem.NormalStrength = 0;
				}
			}

			//switching between camera modes
			if (InputManager::Tab()) {
				controlledCamera->GetComponent<ControlledCamera>()->Active = !controlledCamera->GetComponent<ControlledCamera>()->Active;
				animatedCamera->GetComponent<AnimatedCamera>()->Active = !animatedCamera->GetComponent<AnimatedCamera>()->Active;
			}


			//switching between aliasing modes
			if (InputManager::T()) {
				samples -= 1;
				samples += 8;
				samples %= 8;
				std::cout << "samples: " << samples << std::endl;
				restart = true;
			}
			else if (InputManager::Z()) {
				samples += 1;
				samples += 8;
				samples %= 8;
				std::cout << "samples: " << samples << std::endl;
				restart = true;
			}

			if (InputManager::U()) {
				if (aaEnabled) {
					glDisable(GL_MULTISAMPLE);
				}
				else {

					glEnable(GL_MULTISAMPLE);
				}
				aaEnabled = !aaEnabled;
			}

			glfwSetWindowShouldClose(window, InputManager::Escape());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		for (Entity* e : entities) {
			delete e;
		}
		entities.clear();
		SystemManager::Reset();
		glfwTerminate();
	} while (restart);
	return 0;
}



