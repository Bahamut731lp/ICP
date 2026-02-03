#pragma once

#include "light_system.hpp"
#include "render.hpp"
#include "model.hpp"
#include "scene.hpp"

class World {
public:	
	static void init();
	static Scene calculate(float delta);
	static Scene load(Renderer* window, std::shared_ptr<int> progress);
	static Shader* material;

private:
	static Camera* camera;
	static LightSystem* lights;
	static SpotLight* spotLight;
	static AmbientLight* ambience;
	static PointLight* simpleLight2, * simpleLight3;
	static DirectionalLight* sunlight;
	static Model* coin;
	static Model* terrain;
	static Model* glass;
};
