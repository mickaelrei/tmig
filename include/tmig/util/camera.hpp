#pragma once

#include <glm/glm.hpp>

#include "tmig/render/camera.hpp"

namespace tmig::util {

/// @brief Perform basic first-person camera movement
/// @param firstSinceLast Bool to track if this was called the first time since the last right mouse click
void firstPersonCameraMovement(
    render::Camera& camera,
    float dt,
    bool& firstSinceLast,
    float cameraSpeed = 1.0f,
    float cameraRotationSpeed = 1.0f
);

/// @brief Perform orbital camera movement
/// @param target Target position to orbit around
/// @param radius Radius of the orbit
/// @param azimuth Angle in the XZ plane
/// @param elevation Elevation angle from the Y axis
/// @param firstSinceLast Bool to track if this was called the first time since the last right mouse click
void orbitalCameraMovement(
    render::Camera& camera,
    float dt,
    glm::vec3 target,
    float& radius,
    float& azimuth,
    float& elevation,
    bool& firstSinceLast,
    float cameraSpeed = 1.0f,
    float cameraRotationSpeed = 1.0f
);

} // namespace tmig::util