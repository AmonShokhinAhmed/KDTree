#include "Light.h"
#include "Entity.h"
#include "SystemManager.h"
#include <glm\ext\matrix_clip_space.hpp>
#include <glm\ext\matrix_transform.hpp>
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024
#define NEARPLANE 1.0f
#define FARPLANE 25.0f

Light::Light() :
	AmbientColor(0.5f, 0.5f, 0.5f), DiffuseColor(0.75f, 0.75f, 0.75f), SpecularColor(0.1f, 0.1f, 0.1f),Constant(1.0f),Linear(0.09f), Quadratic(0.032f), _depthMapShader("depthmap.vert", "depthmap.frag", "depthmap.geom"), lightSpaceMatrix(1.0f)
{
	SystemManager::LightSystem.AddComponent(this);
	
	//creating frame buffer for depth map rendering
	glGenFramebuffers(1, &depthMapFBO);

	// create cubemap to store the depthmap
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach cubemap as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

Light::~Light()
{
	SystemManager::LightSystem.RemoveComponent(this);
}

glm::mat4 Light::GetLightSpaceMatrix()
{
	return lightSpaceMatrix;
}

unsigned int Light::GetDepthMap()
{
	return depthCubeMap;
}

float Light::GetNearPlane()
{
	return NEARPLANE;
}

float Light::GetFarPlane()
{
	return FARPLANE;
}

void Light::Update()
{

	//Render Scene form Lights perspective:

	//Generate projection matrix using settings for shadow resolution and near/farplane
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, NEARPLANE, FARPLANE);

	//because we're using point lights, were using a cubemap for the depth map, thus we have to render the scene from 6 different directions, which we set here
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(_owner->transform.Position, _owner->transform.Position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(_owner->transform.Position, _owner->transform.Position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(_owner->transform.Position, _owner->transform.Position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(_owner->transform.Position, _owner->transform.Position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(_owner->transform.Position, _owner->transform.Position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(_owner->transform.Position, _owner->transform.Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	//setting viewport to shadow resolution 
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//activate framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//setup depthmap shader
	_depthMapShader.use();
	for (unsigned int i = 0; i < 6; ++i)
		_depthMapShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	_depthMapShader.setFloat("far_plane", FARPLANE);
	_depthMapShader.setVec3("lightPos", _owner->transform.Position);
	//render the scene
	SystemManager::RendererSystem.RenderScene(_depthMapShader, true);
	//unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
