#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "shader.hpp"

#include "light_ambient.hpp"
#include "light_point.hpp"
#include "light_spot.hpp"
#include "light_directional.hpp"

class LightSystem {
public:
    LightSystem() = default;

    void add(AmbientLight* light);
    void add(PointLight* light);
    void add(SpotLight* light);
    void add(DirectionalLight* light);
    void add(Shader& shader);
    void calc();

private:
    std::vector<Shader> shaders;

    std::vector<AmbientLight*> ambientLights;
    std::vector<PointLight*> pointLights;
    std::vector<SpotLight*> spotLights;
    std::vector<DirectionalLight*> directionalLights;
};