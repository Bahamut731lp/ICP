#include "world.hpp"
#include "logger.hpp"
#include <thread>

Camera* World::camera = nullptr;
Shader* World::material = nullptr;
LightSystem* World::lights = nullptr;
SpotLight* World::spotLight = nullptr;
AmbientLight* World::ambience = nullptr;
PointLight* World::simpleLight2, *World::simpleLight3 = nullptr;
DirectionalLight* World::sunlight = nullptr;
Model* World::coin = nullptr;
Model* World::terrain = nullptr;
Model* World::glass = nullptr;

void World::init()
{
	// Create camera
	camera = new Camera{ glm::vec3(0.0f, 0.0f, 10.0f) };
	GlRender::cam = camera;

	material = new Shader(
		std::filesystem::path("resources/shaders/material.vert"),
		std::filesystem::path("resources/shaders/material.frag")
	);
	
	// Load all models needed for scene
	terrain = new Model("resources/obj/level_1.obj");
	glass = new Model("resources/obj/glass.obj");
	coin = new Model("resources/obj/coin.obj");

	// Define lights
	lights = new LightSystem;
	spotLight = new SpotLight;
	ambience = new AmbientLight;
	simpleLight2 = new PointLight;
	simpleLight3 = new PointLight;
	sunlight = new DirectionalLight;
	
	// Define initial transforms for all objects
	ambience->color = glm::vec3(1.0f);
	ambience->intensity = 0.05f;

	spotLight->position = glm::vec3(50.0f, 12.0f, 2.5f);
	spotLight->direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	spotLight->diffusion = glm::vec3(200.0f);

	simpleLight2->position = glm::vec3(50.0f, 15.0f, 2.5f);
	simpleLight2->diffusion = glm::vec3(30.0f);

	sunlight->ambient = glm::vec3(0.02f);
	sunlight->diffusion = glm::vec3(0.5f);

	// The light is not moving, so we do not have to update position in shader every frame
	lights->add(ambience);
	lights->add(spotLight);
	lights->add(simpleLight2);
	lights->add(sunlight);
	lights->add(*material);
}

Scene World::render(GlRender* GlRender, float delta)
{
	const int ONE_DAY = 16;
	const auto gametime = glfwGetTime();
	auto daytime = glm::sin(((2 * glm::pi<float>() / ONE_DAY) * gametime));

	auto sine_wave = glm::sin(glm::pi<float>() * glfwGetTime());

	// Sunlight movement
	// Send light data to shaders
	simpleLight2->position.x = 30.0f + 20 * daytime;

	coin->transform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 5.0f, 2.5f));
	coin->transform = glm::rotate(coin->transform, (float) glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	glass->transform = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 5 + sine_wave, 2.0f));

	lights->calc();

	terrain->render(*camera, *material);
	glass->render(*camera, *material);
	coin->render(*camera, *material);

	// Process events
	GlRender::cam->onKeyboardEvent(glfwGetCurrentContext(), delta);

	return Scene::SceneWorld;
}

Scene World::load(GlRender* GlRender, std::shared_ptr<int> progress)
{
	// Used to be thread here, but I just gave up trying to make it work with OpenGL.
	if (*progress == 1) {
		World::init();
		glfwSetInputMode(GlRender->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return Scene::SceneWorld;
	}

	*progress = 1;

	return Scene::SceneWorld;
}
