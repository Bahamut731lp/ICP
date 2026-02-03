#include "world.hpp"
#include "player.hpp"
#include "logger.hpp"
#include <thread>
#include <vector>

// Global pointers
static Player *player = nullptr;

// Static World members
Camera *World::camera = nullptr;
Shader *World::material = nullptr;
LightSystem *World::lights = nullptr;
SpotLight *World::spotLight = nullptr;
AmbientLight *World::ambience = nullptr;
PointLight *World::simpleLight2, *World::simpleLight3 = nullptr;
DirectionalLight *World::sunlight = nullptr;
Model *World::coin = nullptr;
Model *World::terrain = nullptr;
Model *World::glass = nullptr;
std::vector<Model*> World::crates;
AudioManager World::audio_manager;
static bool coin_collected = false;
static std::vector<AABB> collisionBoxes;

void World::init()
{
	player = new Player(glm::vec3(0.0f, 10.0f, 10.0f));
	Renderer::camera = &player->camera;

	audio_manager.load_BGM("minecraft", "resources/audio/minecraft_bg.mp3", 1.0f);
	audio_manager.play_BGM("minecraft", 0.1f);

	audio_manager.load("jump", "resources/audio/jump.mp3", 1.0f, 10.0f, 0.8f);
	audio_manager.load("land", "resources/audio/fall.mp3", 10.0f, 50.0f, 0.8f);
	audio_manager.load("soul", "resources/audio/soul.mp3", 3.0f, 20.0f, 1.0f);

	for (int i = 1; i <= 8; i++)
	{
		audio_manager.load("step" + std::to_string(i), "resources/audio/stepdirt_" + std::to_string(i) + ".wav", 1.0f, 10.0f, 0.04f);
	}

	material = new Shader("resources/shaders/material.vert", "resources/shaders/material.frag");
	terrain = new Model("resources/obj/level_1.obj");

    std::vector<glm::vec3> cratePositions = {
        glm::vec3(5.0f, 1.0f, 5.0f),
        glm::vec3(-3.0f, 1.0f, 8.0f),
        glm::vec3(10.0f, 1.0f, -5.0f),
        glm::vec3(-8.0f, 1.0f, -3.0f),
        glm::vec3(15.0f, 1.0f, 10.0f),
        glm::vec3(0.0f, 1.0f, -10.0f),
        glm::vec3(12.0f, 1.0f, 3.0f),
        glm::vec3(-5.0f, 1.0f, -8.0f)
    };

    for (const auto& pos : cratePositions) {
        Model* crate = new Model("resources/obj/Crate1.obj");
        crate->transform = glm::translate(glm::mat4(1.0f), pos);
        crates.push_back(crate);
    }

	glass = new Model("resources/obj/glass.obj");
	coin = new Model("resources/obj/coin.obj");

	// 4. LIGHTS
	lights = new LightSystem;
	spotLight = new SpotLight;
	ambience = new AmbientLight;
	simpleLight2 = new PointLight;
	simpleLight3 = new PointLight;
	sunlight = new DirectionalLight;

	ambience->color = glm::vec3(1.0f);
	ambience->intensity = 0.05f;
	spotLight->position = glm::vec3(50.0f, 12.0f, 2.5f);
	spotLight->direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	spotLight->diffusion = glm::vec3(200.0f);
	simpleLight2->position = glm::vec3(50.0f, 15.0f, 2.5f);
	simpleLight2->diffusion = glm::vec3(30.0f);
	sunlight->ambient = glm::vec3(0.02f);
	sunlight->diffusion = glm::vec3(0.5f);

	lights->add(ambience);
	lights->add(spotLight);
	lights->add(simpleLight2);
	lights->add(sunlight);
	lights->add(*material);
}

Scene World::calculate(float delta)
{
    const int ONE_DAY = 16;
    const auto gametime = glfwGetTime();
    auto daytime = glm::sin(((2 * glm::pi<float>() / ONE_DAY) * gametime));
    auto sine_wave = glm::sin(glm::pi<float>() * glfwGetTime());

    simpleLight2->position.x = 30.0f + 20 * daytime;

    glass->transform = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 5 + sine_wave, 2.0f));

    if (!coin_collected) {
        coin->transform = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.0f, 2.5f)); 
        coin->transform = glm::rotate(coin->transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    }


    collisionBoxes.clear();

    for (auto* crate : crates) {
        collisionBoxes.push_back(crate->calculateAABB());
    }

    collisionBoxes.push_back(glass->calculateAABB());

    if (player) {
        player->update(delta, Renderer::window, audio_manager, collisionBoxes);
    }


    if (!coin_collected) 
    {
        AABB coinBox = coin->calculateAABB();
        if (player && player->getHitbox().intersects(coinBox)) 
        {
            glm::vec3 coinCenter = (coinBox.min + coinBox.max) * 0.5f;
            audio_manager.play_3D("soul", coinCenter.x, coinCenter.y, coinCenter.z);
            
            coin_collected = true; 
        }
    }

    lights->calc();

	terrain->submit(*material);
	glass->submit(*material);
    
    if (!coin_collected) {
		coin->submit(*material);
    }

    return Scene::SceneWorld;
}

Scene World::load(Renderer* Renderer, std::shared_ptr<int> progress)
{
	if (*progress == 1)
	{
		World::init();
		glfwSetInputMode(Renderer->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return Scene::SceneWorld;
	}
	*progress = 1;
	return Scene::SceneWorld;
}