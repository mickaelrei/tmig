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
    float moveSpeed = 10.0f;

    /// @brief Rotation speed
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
    glm::vec3 target{0.0f};

    /// @brief How far from the target the camera is
    float radius = 10.0f;

    /// @brief Movement speed
    float moveSpeed = 10.0f;

    /// @brief Rotation speed
    float rotationSpeed = 1.0f;

    virtual void update(render::Camera& camera, float dt) override;

private:
    bool firstMouse = true;
    float azimuth = 0.0f;
    float elevation = 0.0f;
    glm::vec2 lastMousePos;
};

} // namespace tmig::util