#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace tmig::render {

/// @brief Camera for scene navigation
class Camera {
public:
    /// @brief Constructor
    Camera();

    /// @brief Constructor with initial position
    explicit Camera(const glm::vec3& position);

    /// @brief Set position
    /// @param position Position in world space
    void setPosition(const glm::vec3& position);

    /// @brief Get position in world space
    glm::vec3 getPosition() const;

    /// @brief Move relative to world space
    void move(const glm::vec3& offset);

    /// @brief Move relative to local space
    void moveRelative(const glm::vec3& offset);

    /// @brief Rotate camera with euler angles
    /// @param pitch Angle in radians for world X-axis rotation
    /// @param yaw Angle in radians for world Y-axis rotation
    /// @param roll Angle in radians for world Z-axis rotation
    void rotate(float pitch, float yaw, float roll = 0.0f);

    /// @brief Rotate camera with quaternion
    /// @param rot Quaternion rotation to apply on top of current rotation
    void rotate(const glm::quat& rot);

    /// @brief Set new quaternion rotation
    void setRotation(const glm::quat& rot);

    /// @brief Get current rotation
    glm::quat getRotation() const;

    /// @brief Returns the camera's local forward direction vector
    glm::vec3 getForward() const;

    /// @brief Returns the camera's local right direction vector
    glm::vec3 getRight() const;

    /// @brief Returns the camera's local up direction vector
    glm::vec3 getUp() const;

    /// @brief Get current view matrix
    glm::mat4 getViewMatrix() const;

    /// @brief Look at a target point
    /// @param target Target point in world space
    /// @param up Reference up vector; by default `(0, 1, 0)`
    void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3{0.0f, 1.0f, 0.0f});

    /// @brief Field-of-view (FOV) in degrees
    float fov = 70.0f;

    /// @brief Minimum render distance
    float minDist = 0.01f;

    /// @brief Maximum render distance
    float maxDist = 1000.0f;

private:
    /// @brief Update cached view matrix
    void updateViewMatrix();

    /// @brief Current position
    glm::vec3 pos{0.0f};

    /// @brief Rotation quaternion
    glm::quat rotation = glm::quat{1.0f, glm::vec3{0.0f}};

    /// @brief Cached view matrix
    glm::mat4 viewMatrix;
};

} // namespace tmig::render