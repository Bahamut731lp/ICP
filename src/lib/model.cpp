#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include "model.hpp"
#include "logger.hpp"
#include "obj_loader.hpp"
#include <limits>

Model::Model(const std::filesystem::path& filename)
{
	this->transform = glm::mat4(1.0f);
	this->meshes = std::vector<Mesh>{};

	auto suffix = filename.extension().string();

	if (suffix == ".obj") {
		auto loader = OBJLoader(filename);

		for (Mesh mesh : loader.submeshes) {
			meshes.push_back(mesh);
		}
	}
	else {
		Logger::error("Unrecognized file extension: " + suffix);
	};
}

Model::Model(const Model& copy)
{
	this->meshes = copy.meshes;
	this->transform = glm::mat4(1.0f);
}

Model::Model()
{
}


AABB Model::calculateAABB()
{
    glm::vec3 minBound(std::numeric_limits<float>::max());
    glm::vec3 maxBound(std::numeric_limits<float>::lowest());

    for (const auto& mesh : meshes)
    {
        for (const auto& vertex : mesh.vertices)
        {
            // We transform the local vertex position into world space 
            // using the model's current transform matrix.
            glm::vec4 worldPos = transform * glm::vec4(vertex.Position, 1.0f);

            minBound.x = std::min(minBound.x, worldPos.x);
            minBound.y = std::min(minBound.y, worldPos.y);
            minBound.z = std::min(minBound.z, worldPos.z);

            maxBound.x = std::max(maxBound.x, worldPos.x);
            maxBound.y = std::max(maxBound.y, worldPos.y);
            maxBound.z = std::max(maxBound.z, worldPos.z);
        }
    }
    return AABB{ minBound, maxBound };
}

void Model::render(Camera& camera, Shader& shader)
{
	int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    float aspect = (float)width / (float)height;

	shader.activate();
	//transform = glm::mat4(1.0f);
	shader.setUniform("transform", transform);
	shader.setUniform("view", camera.getViewMatrix());
	shader.setUniform("projection", camera.getProjectionMatrix(aspect));

	for (Mesh mesh : meshes)
	{
		// Set color information
		shader.setUniform("material.diffuse", mesh.material.diffuse);
		shader.setUniform("material.specular", mesh.material.specular);
		shader.setUniform("material.shininess", mesh.material.shininess);
		shader.setUniform("material.transparency", mesh.material.transparency);

		// Texture, if applicable.
		if (mesh.material.texture.id != -1) {
			// We have to send Texture Unit to the uniform
			shader.setUniform("material.texture.textureUnit", 0);
			shader.setUniform("material.texture.isTextured", 1);
			shader.setUniform("material.texture.scale", mesh.material.texture.scale);
            
			//and then activate the texture with it's ID on that texture unit.
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.material.texture.id);
		}
		else {
			shader.setUniform("material.texture.isTextured", 0);
		}

		mesh.draw(shader);

		// Cleanup
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
