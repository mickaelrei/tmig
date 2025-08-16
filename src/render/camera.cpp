#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "tmig/render/camera.hpp"

/// @brief Forward vector in world space
static const glm::vec3 worldForward = glm::vec3{0.0f, 0.0f, -1.0f};

/// @brief Right vector in world space
static const glm::vec3 worldRight = glm::vec3{1.0f, 0.0f, 0.0f};

/// @brief Up vector in world space
static const glm::vec3 worldUp = glm::vec3{0.0f, 1.0f, 0.0f};

namespace tmig::render {

Camera::Camera()
    : Camera::Camera(glm::vec3{0.0f}) {
}

Camera::Camera(const glm::vec3& pos)
    : pos{pos} {
    // Set initial rotation
    const float pitchOffset = 0.01f;
    pitch = glm::clamp(
        pitch,
        -M_PIf * 0.5f + pitchOffset,
        M_PIf * 0.5f - pitchOffset
    );
    rotation = glm::yawPitchRoll(yaw, pitch, roll);
    updateView();
}

void Camera::setPosition(const glm::vec3& position) {
    pos = position;
    updateView();
}

glm::vec3 Camera::getPosition() const {
    return pos;
}

void Camera::move(const glm::vec3& offset) {
    pos += offset;
    updateView();
}

void Camera::moveRelative(const glm::vec3& offset) {
    pos += rotation * offset;
    updateView();
}

void Camera::rotate(float rx, float ry, float rz) {
    pitch += rx;
    yaw += ry;
    roll += rz;

    // Clamp to avoid gimball lock
    const float pitchOffset = 0.01f;
    pitch = glm::clamp(
        pitch,
        -M_PIf * 0.5f + pitchOffset,
        M_PIf * 0.5f - pitchOffset
    );

    // Get new rotation matrix
    rotation = glm::yawPitchRoll(yaw, pitch, roll);

    updateView();
}

void Camera::rotate(float angle, const glm::vec3& axis) {
    rotation = glm::rotate(glm::mat4{rotation}, angle, axis);
    updateView();
}

void Camera::setRotation(const glm::mat3& mat) {
    rotation = mat;
    updateView();
}

glm::mat3 Camera::getRotation() const {
    return rotation;
}

glm::vec3 Camera::forward() const {
    return rotation * worldForward;
}

glm::vec3 Camera::right() const {
    return rotation * worldRight;
}

glm::vec3 Camera::up() const {
    return rotation * worldUp;
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

void Camera::lookAt(
    const glm::vec3& pos,
    const glm::vec3& target,
    const glm::vec3& up
) {
    if (glm::length2(pos - target) == 0.0f) return;

    viewMatrix = glm::lookAt(pos, target, up);

    // To get rotation-matrix from view-matrix, remove position and invert from view to world space
    rotation = glm::translate(viewMatrix, pos);
    rotation = glm::inverse(rotation);
}

void Camera::getPitchYawRoll(float* pitch, float* yaw, float* roll) const {
    if (pitch != nullptr)
        *pitch = Camera::pitch;

    if (yaw != nullptr)
        *yaw = Camera::yaw;

    if (roll != nullptr)
        *roll = Camera::roll;
}

void Camera::setPitchYawRoll(float pitch, float yaw, float roll) {
    Camera::pitch = pitch;
    Camera::yaw = yaw;
    Camera::roll = roll;

    // Clamp to avoid gimball lock
    const float pitchOffset = 0.01f;
    pitch = glm::clamp(
        pitch,
        -M_PIf * 0.5f + pitchOffset,
        M_PIf * 0.5f - pitchOffset
    );

    // Get new rotation matrix
    rotation = glm::yawPitchRoll(yaw, pitch, roll);
    updateView();
}

void Camera::updateView() {
    auto myForward = glm::normalize(rotation * worldForward);
    auto myUp = glm::normalize(rotation * worldUp);

    // Update view matrix
    viewMatrix = glm::lookAt(pos, pos + myForward, myUp);
}

} // namespace tmig::render