#pragma once

#include <glm/glm.hpp>

namespace tmig::render {

/// @brief Camera for scene navigation
class Camera {
public:
    /// @brief Default constructor
    Camera();

    /// @brief Constructor with position
    /// @param pos Initial position
    Camera(const glm::vec3 &pos);

    /// @brief Destructor
    ~Camera() = default;

    /// @brief Set new camera position
    void setPosition(const glm::vec3 &position);

    /// @brief Returns current position
    glm::vec3 getPosition() const;

    /// @brief Move camera by given offset
    /// @param offset Offset in global coordinates
    void move(const glm::vec3 &offset);

    /// @brief Move camear by given offset relative to current rotation
    /// @param offset Offset in global coordinates
    void moveRelative(const glm::vec3 &offset);

    /// @brief Rotate camera by pitch, yaw and roll
    /// @param rx Rotation offset on world X axis in degrees (pitch)
    /// @param ry Rotation offset on world Y axis in degrees (yaw)
    /// @param rz Rotation offset on world Z axis in degrees (roll)
    void rotate(float rx, float ry, float rz = 0.0f);

    /// @brief Rotate with angle-axis mode
    /// @param angle Angle in radians
    /// @param axis Axis to rotate around
    void rotate(float angle, const glm::vec3 &axis);

    /// @brief Set new rotation
    void setRotation(const glm::mat3 &mat);

    /// @brief Get camera's rotation matrix
    glm::mat3 getRotation() const;

    /// @brief Get camera's forward direction vector
    glm::vec3 forward() const;

    /// @brief Get camera's right direction vector
    glm::vec3 right() const;

    /// @brief Get camera's up direction vector
    glm::vec3 up() const;

    /// @brief Get camera view matrix
    glm::mat4 getViewMatrix() const;

    /// @brief Make camera look at target positioned at pos
    /// @param pos New camera position
    /// @param target Camera look target
    /// @param up Optional up vector for camera Z rotation
    void lookAt(
        const glm::vec3 &pos,
        const glm::vec3 &target,
        const glm::vec3 &up = glm::vec3{0.0f, 1.0f, 0.0f}
    );

    /// @brief Get current euler angles in radians
    /// @param pitch Output pitch value
    /// @param yaw Output yaw value
    /// @param roll Output roll value
    void getPitchYawRoll(float *pitch, float *yaw, float *roll) const;

    /// @brief Set new euler angles in radians
    /// @param pitch New pitch (rotation on X)
    /// @param yaw New yaw (rotation on Y)
    /// @param roll New roll (rotation on Z)
    void setPitchYawRoll(float pitch, float yaw, float roll);

    /// @brief Field-of-view (FOV) in degrees
    float fov = 70.0f;

    /// @brief Minimum render distance
    float minDist = 0.01f;

    /// @brief Maximum render distance
    float maxDist = 1000.0f;
private:
    /// @brief Current position
    glm::vec3 pos;

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

} // namespace tmig::render