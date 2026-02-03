#include "player.hpp"
#include "world.hpp" // For AABB definition
#include <iostream>
#include <algorithm>

Player::Player(glm::vec3 startPos)
    : camera(startPos), velocity(0.0f), lastPosition(startPos) {}

AABB Player::getHitbox() const
{
    // Width of 0.6 (0.3 each way) and height of 1.8
    return AABB{
        camera.Position - glm::vec3(0.3f, height, 0.3f),
        camera.Position + glm::vec3(0.3f, 0.0f, 0.3f)};
}

void Player::update(float delta, GLFWwindow *window, AudioManager &audio, const std::vector<AABB> &worldBoxes)
{

    // 1. Audio Listener Setup
    audio.set_listener_position(
        camera.Position.x, camera.Position.y, camera.Position.z,
        camera.Front.x, camera.Front.y, camera.Front.z);

    glm::vec3 posBeforeFrame = camera.Position;

    // --- PHASE A: VERTICAL (Gravity & Landing) ---
    if (!isGrounded)
    {
        velocity.y -= gravity * delta;
    }
    camera.Position.y += velocity.y * delta;

    bool foundGround = false;

    // Check collision against all boxes for floor/ceiling
    for (const auto &box : worldBoxes)
    {
        if (getHitbox().intersects(box))
        {
            // Check if we are falling onto the top of a box
            if (velocity.y <= 0 && posBeforeFrame.y >= box.max.y)
            {
                if (velocity.y < -5.0f)
                {
                    audio.play_3D("land", camera.Position.x, camera.Position.y - height, camera.Position.z);
                }
                camera.Position.y = box.max.y + 0.001f; // Snap to surface
                velocity.y = 0;
                foundGround = true;
            }
            // Check if we hit our head on the bottom of a box
            else if (velocity.y > 0 && posBeforeFrame.y <= box.min.y - height)
            {
                camera.Position.y = box.min.y - 0.01f;
                velocity.y = 0;
            }
        }
    }

    // Basic Terrain Floor Check (Backup)
    if (camera.Position.y < 1.7f)
    {
        if (velocity.y < -4.0f)
        {
            audio.play_3D("land", camera.Position.x, 1.7f - height, camera.Position.z);
        }
        camera.Position.y = 1.7f;
        velocity.y = 0;
        foundGround = true;
    }

    isGrounded = foundGround;

    // Jump logic
    if (isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        velocity.y = jumpForce;
        isGrounded = false;
        audio.play_3D("jump", camera.Position.x, camera.Position.y, camera.Position.z);
    }

    // --- PHASE B: HORIZONTAL (WASD & Walls) ---
    glm::vec3 posBeforeMove = camera.Position;
    camera.onKeyboardEvent(window, delta);

    // Resolution: If moving horizontally caused a collision, undo it
    for (const auto &box : worldBoxes)
    {
        if (getHitbox().intersects(box))
        {
            // Revert X and Z, but keep the Y from the vertical phase
            float currentY = camera.Position.y;
            camera.Position = posBeforeMove;
            camera.Position.y = currentY;
            break;
        }
    }

    // --- PHASE C: FOOTSTEPS ---
    // Only calculate distance on X and Z (horizontal movement)
    float distMoved = glm::distance(glm::vec3(camera.Position.x, 0, camera.Position.z),
                                    glm::vec3(lastPosition.x, 0, lastPosition.z));

    if (isGrounded && distMoved > 0.001f)
    {
        stepTimer += distMoved;
        bool isSprinting = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        float strideLength = isSprinting ? 5.0f : 3.5f;

        if (stepTimer >= strideLength)
        {
            stepTimer = 0.0f;
            int r = (rand() % 8) + 1;
            audio.play("step" + std::to_string(r));
        }
    }

    lastPosition = camera.Position;
}