#include "player.hpp"
#include "world.hpp"
#include <iostream>
#include <algorithm>

Player::Player(glm::vec3 startPos)
    : camera(startPos), velocity(0.0f), lastPosition(startPos) {}

AABB Player::getHitbox() const
{
    return AABB{
        camera.Position - glm::vec3(0.3f, height, 0.3f), // min (feet)
        camera.Position + glm::vec3(0.3f, 0.0f, 0.3f)};  // max (head)
}

void Player::update(float delta, GLFWwindow *window, const std::vector<AABB> &worldBoxes)
{
    glm::vec3 posBeforeFrame = camera.Position;

    if (!isGrounded)
    {
        velocity.y -= gravity * delta;
    }
    camera.Position.y += velocity.y * delta;

    bool foundGround = false;

    for (const auto &box : worldBoxes)
    {
        AABB playerBox = getHitbox();
        bool intersects = playerBox.intersects(box);

        if (intersects)
        {
            if (velocity.y <= 0 && (posBeforeFrame.y - height) >= box.max.y)
            {
                if (velocity.y < -5.0f)
                {
                    //audio.play_3D("land", camera.Position.x, camera.Position.y - height, camera.Position.z);
                }
                camera.Position.y = box.max.y + height + 0.001f;
                velocity.y = 0;
                foundGround = true;
            }
            else if (velocity.y > 0 && posBeforeFrame.y <= box.min.y)
            {
                camera.Position.y = box.min.y - 0.01f;
                velocity.y = 0;
            }
        }
    }

    if (camera.Position.y < 1.7f)
    {
        if (velocity.y < -4.0f)
        {
            //audio.play_3D("land", camera.Position.x, 1.7f - height, camera.Position.z);
        }
        camera.Position.y = 1.7f;
        velocity.y = 0;
        foundGround = true;
    }

    isGrounded = foundGround;

    if (isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        velocity.y = jumpForce;
        isGrounded = false;
        //audio.play_3D("jump", camera.Position.x, camera.Position.y, camera.Position.z);
    }

    glm::vec3 posBeforeMove = camera.Position;
    camera.onKeyboardEvent(window, delta);

    bool hadCollision = false;
    for (const auto &box : worldBoxes)
    {
        AABB playerBox = getHitbox();

        if (playerBox.intersects(box))
        {
            float penetrationLeft = playerBox.max.x - box.min.x;
            float penetrationRight = box.max.x - playerBox.min.x;
            float penetrationFront = playerBox.max.z - box.min.z;
            float penetrationBack = box.max.z - playerBox.min.z;

            float minPenetrationX = std::min(penetrationLeft, penetrationRight);
            float minPenetrationZ = std::min(penetrationFront, penetrationBack);

            if (minPenetrationX < minPenetrationZ)
            {
                if (penetrationLeft < penetrationRight)
                    camera.Position.x = box.min.x - (playerBox.max.x - camera.Position.x) - 0.001f;
                else
                    camera.Position.x = box.max.x + (camera.Position.x - playerBox.min.x) + 0.001f;
            }
            else
            {
                if (penetrationFront < penetrationBack)
                    camera.Position.z = box.min.z - (playerBox.max.z - camera.Position.z) - 0.001f;
                else
                    camera.Position.z = box.max.z + (camera.Position.z - playerBox.min.z) + 0.001f;
            }

            hadCollision = true;
            break;
        }
    }

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
            //audio.play("step" + std::to_string(r));
        }
    }

    lastPosition = camera.Position;
}