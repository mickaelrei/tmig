#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace tmig {

namespace render {

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

    /// @brief Rotate camera by pitch, yaw and roll
    /// @param rx rotation offset on world X axis in degrees (pitch)
    /// @param ry rotation offset on world Y axis in degrees (yaw)
    /// @param rz rotation offset on world Z axis in degrees (roll)
    void rotate(float rx, float ry, float rz = 0.0f);

    /// @brief Rotate with angle-axis mode
    /// @param angle angle in radians
    /// @param axis axis to rotate around
    void rotate(float angle, const glm::vec3 &axis);

    /// @brief Set new rotation
    /// @param mat rotation matrix
    void setRotation(const glm::mat3 &mat);

    /// @brief Get camera's rotation matrix
    /// @return mat3 for rotation matrix
    glm::mat3 getRotation() const;

    /// @brief Get camera's forward direction vector
    /// @return vec3 for forward direction
    glm::vec3 forward() const;

    /// @brief Get camera's right direction vector
    /// @return vec3 for right direction
    glm::vec3 right() const;

    /// @brief Get camera's up direction vector
    /// @return vec3 for up direction
    glm::vec3 up() const;

    /// @brief Make camera look at [target] positioned at [pos]
    /// @param pos new camera position
    /// @param target camera look target
    /// @param up optional up vector for camera Z rotation
    void lookAt(
        const glm::vec3 &pos,
        const glm::vec3 &target,
        const glm::vec3 &up = glm::vec3{0.0f, 1.0f, 0.0f}
    );

    /// @brief Get current euler angles in radians
    /// @param pitch output pitch value
    /// @param yaw output yaw value
    /// @param roll output roll value
    void getPitchYawRoll(float *pitch, float *yaw, float *roll) const;

    /// @brief Set new euler angles in radians
    /// @param pitch new pitch (rotation on X)
    /// @param yaw new yaw (rotation on Y)
    /// @param roll new roll (rotation on Z)
    void setPitchYawRoll(float pitch, float yaw, float roll);

    /// @brief Current position
    glm::vec3 pos;

    /// @brief Field-of-view (FOV) in degrees
    float fov = 70.0f;

    /// @brief Minimum render distance
    float minDist = 0.01f;

    /// @brief Maximum render distance
    float maxDist = 1000.0f;

    /// @brief Movement speed
    float moveSpeed = 3.0f;

    /// @brief Rotation speed
    float rotationSpeed = 0.25f;

    /// @brief Whether it is the first click on the window (used for rotation with mouse)
    bool firstClick = true;

private:
    /// @brief Cached view matrix
    glm::mat4 viewMatrix;

    /// @brief Current pitch in radians (rotation on X axis)
    float pitch = 0.0f;

    /// @brief Current yaw in radians (rotation on Y axis)
    float yaw = 0.0f;

    /// @brief Current roll in radians (rotation on Z axis)
    float roll = 0.0f;

    /// @brief Rotation matrix
    glm::mat3 rotation = glm::mat3{1.0f};

    /// @brief Update cached view matrix
    void updateView();
};

} // namespace render

} // namespace tmig