#pragma once

#include "tmig/render/camera.hpp"

namespace tmig::util {

/// @brief Base interface for a camera controller
class CameraController {
public:
    virtual ~CameraController() = default;

    /// @brief Updates the camera's state
    /// @param camera The camera to be controlled
    /// @param dt The delta time for frame-rate independent movement
    virtual void update(render::Camera& camera, float dt) = 0;
};

/// @brief A controller for first-person camera movement
class FirstPersonCameraController : public CameraController {
public:
    /// @brief Movement speed
    /// @note A negative value will make the camera move in opposite directions
    float moveSpeed = 10.0f;

    /// @brief Rotation speed
    /// @note A negative value will make the camera rotate in opposite directions
    float rotationSpeed = 1.0f;

    virtual void update(render::Camera& camera, float dt) override;

private:
    bool firstMouse = true;
    float yaw = 0.0f;
    float pitch = 0.0f;
    glm::vec2 lastMousePos;
};

/// @brief A controller for an orbital camera that rotates around a target
class OrbitalCameraController : public CameraController {
public:
    /// @brief Target to orbit around
    /// @note This can be changed at any time
    glm::vec3 target{0.0f};

    /// @brief How far from the target the camera is
    /// @note This is automatically changed by a call to `update` if the
    /// movement keys are pressed, but you change it at any time
    float radius = 10.0f;

    /// @brief Movement speed for zooming
    /// @note A negative value will make the camera move in opposite directions
    float moveSpeed = 10.0f;

    /// @brief Rotation speed
    /// @note A negative value will make the camera rotate in opposite directions
    float rotationSpeed = 1.0f;

    virtual void update(render::Camera& camera, float dt) override;

private:
    bool firstMouse = true;
    float azimuth = 0.0f;
    float elevation = 0.0f;
    glm::vec2 lastMousePos;
};

/// @brief A controller for smooth first-person camera movement
class SmoothFirstPersonCameraController : public CameraController {
public:
    /// @brief Movement speed
    /// @note A negative value will make the camera move in opposite directions
    float moveSpeed = 10.0f;

    /// @brief Rotation speed
    /// @note A negative value will make the camera rotate in opposite directions
    float rotationSpeed = 1.0f;

    /// @brief Smoothing factor for movement and rotation (lerp alpha)
    /// @note Should be in the (0, 1) interval, where 0 means static
    /// (won't move/rotate at all) and 1 is instant movement/rotation
    float smoothness = 0.1f;

    virtual void update(render::Camera& camera, float dt) override;

private:
    bool firstMouse = true;
    glm::vec3 targetPosition;
    glm::quat targetRotation;
    glm::vec2 lastMousePos;
    float yaw = 0.0f;
    float pitch = 0.0f;
};

/// @brief A controller for smooth orbital camera movement
class SmoothOrbitalCameraController : public CameraController {
public:
    /// @brief Target to orbit around
    /// @note This can be changed at any time
    glm::vec3 target{0.0f};

    /// @brief How far from the target the camera is
    /// @note This is automatically changed by a call to `update` if the
    /// movement keys are pressed, but you change it at any time
    float radius = 10.0f;

    /// @brief Movement speed for zooming
    /// @note A negative value will make the camera move in opposite directions
    float moveSpeed = 10.0f;

    /// @brief Rotation speed
    /// @note A negative value will make the camera rotate in opposite directions
    float rotationSpeed = 1.0f;

    /// @brief Smoothing factor for movement and rotation (lerp alpha)
    /// @note Should be in the (0, 1) interval, where 0 means static
    /// (won't move/rotate at all) and 1 is instant movement/rotation
    float smoothness = 0.1f;

    virtual void update(render::Camera& camera, float dt) override;

private:
    bool firstMouse = true;
    glm::vec2 lastMousePos;
    float targetRadius;
    float targetAzimuth = 0.0f;
    float targetElevation = 0.0f;
    float currentRadius;
    float currentAzimuth = 0.0f;
    float currentElevation = 0.0f;
};

} // namespace tmig::util