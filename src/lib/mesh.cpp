#include "mesh.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Mesh::Mesh(GLenum primitive_type, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint texture_id) : 
    primitive_type(primitive_type),
    vertices(vertices),
    indices(indices),
    texture_id(texture_id)
{
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferStorage(VBO, vertices.size() * sizeof(Vertex), vertices.data(), 0); // 0 for static, or use GL_DYNAMIC_STORAGE_BIT
    glNamedBufferStorage(EBO, indices.size() * sizeof(GLuint), indices.data(), 0);

    glVertexArrayElementBuffer(VAO, EBO);
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribBinding(VAO, 0, 0); // Map attribute 0 to binding point 0

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
    glVertexArrayAttribBinding(VAO, 1, 0); // Map attribute 1 to binding point 0

    glEnableVertexArrayAttrib(VAO, 2);
    glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, UVs));
    glVertexArrayAttribBinding(VAO, 2, 0); // Map attribute 2 to binding point 0
}

void Mesh::draw(Shader& shader)
{
	shader.activate();

	glBindVertexArray(VAO);
    glDrawElements(primitive_type, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

}

void Mesh::clear()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);

    vertices.clear();
    indices.clear();
    VAO = VBO = EBO = 0;
}
