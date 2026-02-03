#pragma once
#include <filesystem>
#include <vector>
#include "physics.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include "camera.hpp"
#include "mesh.hpp"

class Model
{
public:
	glm::mat4 transform;
	std::vector<Mesh> meshes;

	Model(const std::filesystem::path& filename);
	Model(const Model& copy);
	Model();

	void render(Camera& camera, Shader& shader);
	AABB calculateAABB();
};

