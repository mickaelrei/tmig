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
    if (isKeyDown(core::input::Key::SPACE))
        displacement.y += 1.0f;
    if (isKeyDown(core::input::Key::LEFT_SHIFT))
        displacement.y -= 1.0f;

    if (glm::length2(displacement) > 0.0f) {
        camera.moveRelative(glm::normalize(displacement) * moveSpeed * dt);
    }

    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::RIGHT)) {
        setCursorMode(core::input::CursorMode::DISABLED);

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
        setCursorMode(core::input::CursorMode::NORMAL);
        firstMouse = true;
    }
}

void OrbitalCameraController::update(render::Camera& camera, float dt) {
    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::RIGHT)) {
        setCursorMode(core::input::CursorMode::DISABLED);

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
        setCursorMode(core::input::CursorMode::NORMAL);
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

void SmoothFirstPersonCameraController::update(render::Camera& camera, float dt) {
    auto currentMousePos = core::input::getCursorPos();

    // Initialize target position and rotation on the first update
    if (firstMouse) {
        targetPosition = camera.getPosition();
        targetRotation = camera.getRotation();
        lastMousePos = currentMousePos;
        firstMouse = false;
    }

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
    if (isKeyDown(core::input::Key::SPACE))
        displacement.y += 1.0f;
    if (isKeyDown(core::input::Key::LEFT_SHIFT))
        displacement.y -= 1.0f;

    if (glm::length2(displacement) > 0.0f) {
        targetPosition += targetRotation * glm::normalize(displacement) * moveSpeed * dt;
    }

    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::RIGHT)) {
        setCursorMode(core::input::CursorMode::DISABLED);

        // Calculate the offset from the last frame
        glm::vec2 offset = currentMousePos - lastMousePos;

        if (offset.x != 0.0f || offset.y != 0.0f) {
            // Update yaw and pitch angles based on mouse movement
            yaw   -= offset.x * rotationSpeed * .001f;
            pitch -= offset.y * rotationSpeed * .001f;

            // Clamp the pitch to prevent looking backwards
            pitch = glm::clamp(pitch, -1.55334f, 1.55334f);

            // Construct the new orientation from yaw and the clamped pitch
            glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3{0.0f, 1.0f, 0.0f});
            glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3{1.0f, 0.0f, 0.0f});

            targetRotation = glm::normalize(yawQuat * pitchQuat);
        }
    } else {
        setCursorMode(core::input::CursorMode::NORMAL);
    }

    lastMousePos = currentMousePos;

    // Smoothly interpolate the camera's position and rotation
    camera.setPosition(glm::mix(camera.getPosition(), targetPosition, smoothness));
    camera.setRotation(glm::slerp(camera.getRotation(), targetRotation, smoothness));
}

void SmoothOrbitalCameraController::update(render::Camera& camera, float dt) {
    auto currentMousePos = core::input::getCursorPos();

    if (firstMouse) {
        targetRadius = radius;
        currentRadius = radius;
        lastMousePos = currentMousePos;
        firstMouse = false;
    }

    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::RIGHT)) {
        setCursorMode(core::input::CursorMode::DISABLED);

        // Calculate the offset from the last frame
        glm::vec2 offset = currentMousePos - lastMousePos;

        // Update angles based on mouse movement
        targetAzimuth -= offset.x * rotationSpeed * 0.001f;
        targetElevation += offset.y * rotationSpeed * 0.001f;

        // Clamp to prevent looking backwards
        targetElevation = glm::clamp(targetElevation, -1.55334f, 1.55334f);

    } else {
        setCursorMode(core::input::CursorMode::NORMAL);
    }

    // Zoom
    if (isKeyDown(core::input::Key::W)) {
        targetRadius -= dt * moveSpeed;
    }
    if (isKeyDown(core::input::Key::S)) {
        targetRadius += dt * moveSpeed;
    }
    targetRadius = glm::max(targetRadius, 1.0f);

    // Smoothly interpolate values
    currentRadius = glm::mix(currentRadius, targetRadius, smoothness);
    currentAzimuth = glm::mix(currentAzimuth, targetAzimuth, smoothness);
    currentElevation = glm::mix(currentElevation, targetElevation, smoothness);

    // Update camera position and orientation
    float x = target.x + currentRadius * std::cos(currentElevation) * std::sin(currentAzimuth);
    float y = target.y + currentRadius * std::sin(currentElevation);
    float z = target.z + currentRadius * std::cos(currentElevation) * std::cos(currentAzimuth);

    camera.setPosition({x, y, z});
    camera.lookAt(target);

    lastMousePos = currentMousePos;
}

void SmoothFreeFlyCameraController::update(render::Camera& camera, float dt) {
    auto currentMousePos = core::input::getCursorPos();

    // Initialize target position and rotation on the first update
    if (firstMouse) {
        targetPosition = camera.getPosition();
        targetRotation = camera.getRotation();
        lastMousePos = currentMousePos;
        firstMouse = false;
    }

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
    if (isKeyDown(core::input::Key::SPACE))
        displacement.y += 1.0f;
    if (isKeyDown(core::input::Key::LEFT_SHIFT))
        displacement.y -= 1.0f;

    if (glm::length2(displacement) > 0.0f) {
        targetPosition += targetRotation * glm::normalize(displacement) * moveSpeed * dt;
    }

    // Rotation
    if (isMouseButtonDown(core::input::MouseButton::RIGHT)) {
        setCursorMode(core::input::CursorMode::DISABLED);

        // Calculate the offset from the last frame
        glm::vec2 offset = currentMousePos - lastMousePos;

        if (offset.x != 0.0f || offset.y != 0.0f) {
            // Get the camera's local right and up vectors to apply pitch and yaw correctly
            glm::vec3 right = glm::normalize(targetRotation * glm::vec3(1.0f, 0.0f, 0.0f));
            glm::vec3 up = glm::normalize(targetRotation * glm::vec3(0.0f, 1.0f, 0.0f));

            // Create quaternions for pitch and yaw
            glm::quat pitchQuat = glm::angleAxis(-offset.y * rotationSpeed * 0.001f, right);
            glm::quat yawQuat = glm::angleAxis(-offset.x * rotationSpeed * 0.001f, up);

            // Apply yaw and pitch to the target rotation
            targetRotation = glm::normalize(yawQuat * pitchQuat * targetRotation);
        }
    } else {
        setCursorMode(core::input::CursorMode::NORMAL);
    }

    // Roll
    float rollDeta = 0.0f;
    if (isKeyDown(core::input::Key::Q))
        rollDeta -= rollSpeed * dt;
    if (isKeyDown(core::input::Key::E))
        rollDeta += rollSpeed * dt;

    glm::vec3 forward = glm::normalize(targetRotation * glm::vec3(0.0f, 0.0f, -1.0f));
    glm::quat rollQuat = glm::angleAxis(rollDeta, forward);
    targetRotation = glm::normalize(rollQuat * targetRotation);

    lastMousePos = currentMousePos;

    // Smoothly interpolate the camera's position and rotation
    camera.setPosition(glm::mix(camera.getPosition(), targetPosition, smoothness));
    camera.setRotation(glm::slerp(camera.getRotation(), targetRotation, smoothness));
}

} // namespace tmig::util