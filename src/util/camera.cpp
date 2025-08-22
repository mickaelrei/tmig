#include <GLFW/glfw3.h>

#include "tmig/util/camera.hpp"
#include "tmig/render/window.hpp"

namespace tmig::util {

void firstPersonCameraMovement(
    render::Camera& camera,
    float dt,
    bool& firstSinceLast,
    float cameraSpeed,
    float cameraRotationSpeed
) {
    // Camera movement
    glm::vec3 displacement{0.0f};
    if (render::window::getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
        displacement.z -= dt;
    }
    if (render::window::getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
        displacement.z += dt;
    }
    if (render::window::getKeyState(GLFW_KEY_D) == GLFW_PRESS) {
        displacement.x += dt;
    }
    if (render::window::getKeyState(GLFW_KEY_A) == GLFW_PRESS) {
        displacement.x -= dt;
    }
    if (render::window::getKeyState(GLFW_KEY_SPACE) == GLFW_PRESS) {
        displacement.y += dt;
    }
    if (render::window::getKeyState(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        displacement.y -= dt;
    }
    camera.moveRelative(displacement * cameraSpeed);

    // Camera rotation
    if (render::window::getMouseButtonState(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        render::window::setCursorMode(GLFW_CURSOR_DISABLED);

        // Get screen center
        auto screenCenter = glm::vec2{render::window::getSize()} * 0.5f;

        // If first click, reset to center
        if (firstSinceLast) {
            firstSinceLast = false;
            render::window::setCursorPos(screenCenter);
        }

        // Get offset from center and rotate
        auto pos = render::window::getCursorPos();
        auto offset = glm::vec2{pos} - screenCenter;
        camera.rotate(-offset.y * cameraRotationSpeed * dt, -offset.x * cameraRotationSpeed * dt);

        // Set cursor back to center
        render::window::setCursorPos(screenCenter);
    } else {
        // Show cursor again
        render::window::setCursorMode(GLFW_CURSOR_NORMAL);
        firstSinceLast = true;
    }
}

void orbitalCameraMovement(
    render::Camera& camera,
    float dt,
    glm::vec3 target,
    float& radius,
    float& azimuth,
    float& elevation,
    bool& firstSinceLast,
    float cameraSpeed,
    float cameraRotationSpeed
) {
    // Camera rotation (azimuth and elevation)
    if (render::window::getMouseButtonState(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        render::window::setCursorMode(GLFW_CURSOR_DISABLED);

        // Get screen center
        auto screenCenter = glm::vec2{render::window::getSize()} * 0.5f;

        // If first click, reset to center
        if (firstSinceLast) {
            firstSinceLast = false;
            render::window::setCursorPos(screenCenter);
        }

        // Get offset from center and update angles
        auto pos = render::window::getCursorPos();
        auto offset = glm::vec2{pos} - screenCenter;
        azimuth -= offset.x * cameraRotationSpeed * dt;
        elevation += offset.y * cameraRotationSpeed * dt;

        // Clamp elevation to avoid flipping
        elevation = glm::clamp(elevation, -M_PIf * 0.5f + 0.1f, M_PIf * 0.5f - 0.1f);

        // Set cursor back to center
        render::window::setCursorPos(screenCenter);
    } else {
        // Show cursor again
        render::window::setCursorMode(GLFW_CURSOR_NORMAL);
        firstSinceLast = true;
    }

    // Camera zoom (adjust radius)
    if (render::window::getKeyState(GLFW_KEY_W) == GLFW_PRESS) {
        radius -= dt * cameraSpeed;
    }
    if (render::window::getKeyState(GLFW_KEY_S) == GLFW_PRESS) {
        radius += dt * cameraSpeed;
    }
    radius = glm::max(radius, 1.0f); // Prevent radius from going negative or too small

    // Calculate new camera position using spherical coordinates
    float x = target.x + radius * std::cos(elevation) * std::sin(azimuth);
    float y = target.y + radius * std::sin(elevation);
    float z = target.z + radius * std::cos(elevation) * std::cos(azimuth);

    camera.lookAt(glm::vec3{x, y, z}, target);
}

} // namespace tmig::util