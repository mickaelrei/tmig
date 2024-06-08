#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    Camera(const glm::vec3 &pos);
    ~Camera() = default;

    glm::mat4 getViewMatrix() const;

    void moveForward(float dt);
    void moveBack(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    void moveUp(float dt);
    void moveDown(float dt);

    void rotate(float rx, float ry, float dt);

    glm::vec3 pos;
    glm::vec3 forward = glm::vec3{0.0f, 0.0f, -1.0f};
    glm::vec3 up = glm::vec3{0.0f, 1.0f, 0.0f};
    glm::vec3 right = glm::vec3{1.0f, 0.0f, 0.0f};

    float fov = 70.0f;
    float minDist = 0.01f;
    float maxDist = 1000.0f;

    float moveSpeed = 3.0f;
    float rotationSpeed = 100.0f;

private:
    void updateVectors();

    glm::mat4 viewMatrix;
    glm::vec3 worldUp = glm::vec3{0.0f, 1.0f, 0.0f};
    float pitch = 0.0f;
    float yaw = -90.0f;
};