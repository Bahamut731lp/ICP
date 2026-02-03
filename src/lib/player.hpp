#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "camera.hpp"
#include "audio/audio_manager.hpp"
#include "physics.hpp" 

class Player {
public:
    Player(glm::vec3 startPos);

    void update(float delta, GLFWwindow* window, AudioManager& audio, const std::vector<AABB>& worldBoxes);
    AABB getHitbox() const;

    Camera camera;
    glm::vec3 velocity;
    glm::vec3 lastPosition;

private:
    float gravity = 20.0f;
    float jumpForce = 8.0f;
    float height = 1.8f;
    float stepTimer = 0.0f;
    bool isGrounded = false;
};