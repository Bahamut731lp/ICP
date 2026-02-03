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

AudioManager World::audio_manager;


static bool coin_collected = false;
void World::init()
{
	// Create camera
	camera = new Camera{ glm::vec3(0.0f, 0.0f, 10.0f) };
	GlRender::cam = camera;

	material = new Shader(
		std::filesystem::path("resources/shaders/material.vert"),
		std::filesystem::path("resources/shaders/material.frag")
	);

	// audio
	audio_manager.load("soul", "resources/audio/soul.mp3", 3.0f, 20.0f, 1.0f);
	audio_manager.load_BGM("minecraft", "resources/audio/minecraft_bg.mp3", 1.0f);
	audio_manager.play_BGM("minecraft", 0.1f);
	
	// Load all models needed for scene
	terrain = new Model("resources/obj/level_1.obj");
	glass = new Model("resources/obj/glass.obj");
	coin = new Model("resources/obj/coin.obj");

	for (int i = 1; i <= 8; i++) 
    {
        std::string id = "step" + std::to_string(i);
        std::string path = "resources/audio/stepdirt_" + std::to_string(i) + ".wav";
        
        audio_manager.load(id, path, 1.0f, 10.0f, 0.04f);
    }

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

static glm::vec3 last_position = glm::vec3(0.0f);
static float step_timer = 0.0f;
static float step_interval = 0.5f; // Play a sound every 0.5 seconds
Scene World::render(GlRender* GlRender, float delta)
{
    const int ONE_DAY = 16;
    const auto gametime = glfwGetTime();
    auto daytime = glm::sin(((2 * glm::pi<float>() / ONE_DAY) * gametime));
    auto sine_wave = glm::sin(glm::pi<float>() * glfwGetTime());

	audio_manager.set_listener_position(
        camera->Position.x, camera->Position.y, camera->Position.z,
        camera->Front.x,    camera->Front.y,    camera->Front.z
    );

	// Check if player moved
    float distance_moved = glm::distance(camera->Position, last_position);
    bool is_moving = distance_moved > 0.001f; 

    if (is_moving)
    {
        step_timer -= delta; 

        if (step_timer <= 0.0f)
        {
            int r = (rand() % 8) + 1; 
            std::string sound_name = "step" + std::to_string(r);

            audio_manager.play_3D(sound_name, camera->Position.x, camera->Position.y- 3.7f, camera->Position.z);

            bool is_sprinting = glfwGetKey(GlRender->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            step_timer = is_sprinting ? 0.3f : 0.5f;
        }
    }
    else
    {
        step_timer = 0.0f; 
    }

    last_position = camera->Position;


    simpleLight2->position.x = 30.0f + 20 * daytime;
    glass->transform = glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 5 + sine_wave, 2.0f));

    glm::vec3 coinPos = glm::vec3(-2.0f, 1.0f, 2.5f);
    if (!coin_collected) 
    {
        coin->transform = glm::translate(glm::mat4(1.0f), coinPos);
        coin->transform = glm::rotate(coin->transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        float dist = glm::distance(camera->Position, coinPos);
        if (dist < 4.0f) 
        {
            audio_manager.play_3D("soul", coinPos.x, coinPos.y, coinPos.z);
            coin_collected = true; 
        }
    }

    lights->calc();

    terrain->render(*camera, *material);
    glass->render(*camera, *material);
    
    if (!coin_collected) {
        coin->render(*camera, *material);
    }

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
