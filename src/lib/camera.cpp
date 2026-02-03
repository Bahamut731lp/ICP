#include <iostream>
#include "camera.hpp"
#include "logger.hpp"
#include "render.hpp"
#include "glm/common.hpp"
#include "glm/ext.hpp"

Camera::Camera(glm::vec3 position)
	: Position(position)
{
	this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->Right = glm::vec3(0.0f, 0.0f, 0.0f);
	this->WorldUp = glm::vec3(0.0f, 0.0f, 0.0f);

	this->Yaw = -90.0f;
	this->Pitch = 30.0f;
	this->Roll = 30.0f;
	this->Zoom = 0.0f;

	this->MovementSpeed = 10.0f;
	this->SprintFactor = 3.0f;
	this->MouseSensitivity = 0.1f;
	this->mode = Camera_Mode::FIRST_PERSON;

	// initialization of the camera reference system
	this->updateCameraVectors();
}

Camera::Camera()
{
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) {
	return glm::perspective(glm::radians(FOV), 1920.0f / 1080.0f, 0.1f, 100.0f);
}

void Camera::onKeyboardEvent(GLFWwindow* window, GLfloat deltaTime)
{
    float cameraSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? SprintFactor : 1) * MovementSpeed * deltaTime;

    glm::vec3 flatFront = glm::normalize(glm::vec3(this->Front.x, 0.0f, this->Front.z));
    glm::vec3 flatRight = glm::normalize(glm::vec3(this->Right.x, 0.0f, this->Right.z));
	
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		Renderer::setCursor(FREE);
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		Renderer::setCursor(LOCKED);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        this->Position += cameraSpeed * flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->Position -= cameraSpeed * flatFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->Position -= cameraSpeed * flatRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->Position += cameraSpeed * flatRight;
    }
}

void Camera::onMouseEvent(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
{
	if (this->mode == Camera_Mode::STATIC) {
		return;
	}

	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw += xoffset;
	this->Pitch += yoffset;

	if (constraintPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}

	this->updateCameraVectors();
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}