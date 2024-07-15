#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace tmig {

/// @brief Camera for scene navigation
class Camera {
public:
    /// @brief Default constructor
    Camera();

    /// @brief Constructor with position
    /// @param pos initial position
    Camera(const glm::vec3 &pos);

    /// @brief Destructor
    ~Camera() = default;

    /// @brief Get camera view matrix
    /// @return mat4 for camera view
    glm::mat4 getViewMatrix() const;

    /// @brief Move camera forward
    /// @param dt delta time
    void moveForward(float dt);

    /// @brief Move camera back
    /// @param dt delta time
    void moveBack(float dt);

    /// @brief Move camera left
    /// @param dt delta time
    void moveLeft(float dt);

    /// @brief Move camera right
    /// @param dt delta time
    void moveRight(float dt);

    /// @brief Move camera up
    /// @param dt delta time
    void moveUp(float dt);

    /// @brief Move camera down
    /// @param dt delta time
    void moveDown(float dt);

    /// @brief Rotate camera
    /// @param rx rotation offset on X axis
    /// @param ry rotation offset on Y axis
    /// @param dt delta time
    void rotate(float rx, float ry, float dt);

    /// @brief Get camera's forward direction vector
    /// @return vec3 for forward direction
    glm::vec3 getForward() const;

    /// @brief Current position
    glm::vec3 pos;

    /// @brief Field-of-view (FOV)
    float fov = 70.0f;

    /// @brief Minimum render distance
    float minDist = 0.01f;

    /// @brief Maximum render distance
    float maxDist = 1000.0f;

    /// @brief Movement speed
    float moveSpeed = 3.0f;

    /// @brief Rotation speed
    float rotationSpeed = 100.0f;

private:
    /// @brief Camera forward
    glm::vec3 forward = glm::vec3{0.0f, 0.0f, -1.0f};

    /// @brief Camera up
    glm::vec3 up = glm::vec3{0.0f, 1.0f, 0.0f};

    /// @brief Camera right
    glm::vec3 right = glm::vec3{1.0f, 0.0f, 0.0f};

    /// @brief Cached view matrix
    glm::mat4 viewMatrix;

    /// @brief World up vector
    glm::vec3 worldUp = glm::vec3{0.0f, 1.0f, 0.0f};

    /// @brief Current pitch (rotation on X axis)
    float pitch = 0.0f;

    /// @brief Current yaw (rotation on Y axis)
    float yaw = -90.0f;

    /// @brief Update vectors after rotation changes
    void updateVectors();
};

} // namespace tmig