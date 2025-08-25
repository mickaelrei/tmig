#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "tmig/util/camera_controller.hpp"
#include "tmig/core/input.hpp"

namespace tmig::util {

void FirstPersonCameraController::update(render::Camera& camera, float dt) {
    // Movement
    glm::vec3 displacement{0.0f};
    if (isKeyDown(core::input::Key::W))
        displacement.z -= 1.0f;
    if (isKeyDown(core::input::Key::S))
        displacement.z += 1.0f;
    if (isKeyDown(core::input::Key::D))
        displacement.x += 1.0f;
    if (isKeyDown(core::input::Key::A))
        displacement.x -= 1.0f;
    if (isKeyDown(core::input::Key::Space))
        displacement.y += 1.0f;
    if (isKeyDown(core::input::Key::LeftShift))
        displacement.y -= 1.0f;
    
    if (glm::length2(displacement) > 0.0f) {
        camera.moveRelative(glm::normalize(displacement) * moveSpeed * dt);
    }

    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::Right)) {
        setCursorMode(core::input::CursorMode::Disabled);

        auto currentMousePos = core::input::getCursorPos();

        if (firstMouse) {
            lastMousePos = currentMousePos;
            firstMouse = false;
        }

        // Calculate the offset from the last frame
        glm::vec2 offset = currentMousePos - lastMousePos;
        lastMousePos = currentMousePos;

        if (offset.x != 0.0f || offset.y != 0.0f) {
            // Update yaw and pitch angles based on mouse movement
            yaw   -= offset.x * rotationSpeed * .001f;
            pitch -= offset.y * rotationSpeed * .001f;

            // Clamp the pitch to prevent looking backwards
            pitch = glm::clamp(pitch, -1.55334f, 1.55334f);

            // Construct the new orientation from yaw and the clamped pitch
            glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3{0.0f, 1.0f, 0.0f});
            glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3{1.0f, 0.0f, 0.0f});

            camera.setRotation(yawQuat * pitchQuat);
        }
    } else {
        setCursorMode(core::input::CursorMode::Normal);
        firstMouse = true;
    }
}

void OrbitalCameraController::update(render::Camera& camera, float dt) {
    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::Right)) {
        setCursorMode(core::input::CursorMode::Disabled);

        auto currentMousePos = core::input::getCursorPos();

        if (firstMouse) {
            lastMousePos = currentMousePos;
            firstMouse = false;
        }

        // Calculate the offset from the last frame
        glm::vec2 offset = currentMousePos - lastMousePos;
        lastMousePos = currentMousePos;

        // Update angles based on mouse movement
        azimuth -= offset.x * rotationSpeed * 0.001f;
        elevation += offset.y * rotationSpeed * 0.001f;

        // Clamp to prevent looking backwards
        elevation = glm::clamp(elevation, -1.55334f, 1.55334f);
    } else {
        setCursorMode(core::input::CursorMode::Normal);
        firstMouse = true;
    }

    // Zoom
    if (isKeyDown(core::input::Key::W)) {
        radius -= dt * moveSpeed;
    }
    if (isKeyDown(core::input::Key::S)) {
        radius += dt * moveSpeed;
    }
    radius = glm::max(radius, 1.0f);

    // Update camera position and orientation
    float x = target.x + radius * std::cos(elevation) * std::sin(azimuth);
    float y = target.y + radius * std::sin(elevation);
    float z = target.z + radius * std::cos(elevation) * std::cos(azimuth);

    camera.setPosition({x, y, z});
    camera.lookAt(target);
}

} // namespace tmig::util