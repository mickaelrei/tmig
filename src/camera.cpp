#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

#include "tmig/camera.hpp"

namespace tmig {

Camera::Camera()
    : Camera::Camera(glm::vec3{0.0f})
{
}

Camera::Camera(const glm::vec3 &pos)
    : pos{pos}
{
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return viewMatrix;
}

void Camera::moveForward(float dt)
{
    pos += forward * moveSpeed * dt;
}

void Camera::moveBack(float dt)
{
    pos -= forward * moveSpeed * dt;
}

void Camera::moveLeft(float dt)
{
    pos -= right * moveSpeed * dt;
}

void Camera::moveRight(float dt)
{
    pos += right * moveSpeed * dt;
}

void Camera::moveUp(float dt)
{
    pos += up * moveSpeed * dt;
}

void Camera::moveDown(float dt)
{
    pos -= up * moveSpeed * dt;
}

void Camera::rotate(float rx, float ry)
{
    yaw += ry * rotationSpeed;
    pitch += rx * rotationSpeed;

    // Clamp to avoid gimball lock
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

glm::vec3 Camera::getForward() const {
    return forward;
}

void Camera::setForward(const glm::vec3 &forward) {
    if (glm::length2(forward) == 0.0f) return;

    Camera::forward = glm::normalize(forward);
    updateVectors(false);
}

float Camera::getPitch() const {
    return pitch;
}

float Camera::getYaw() const {
    return yaw;
}

void Camera::updateVectors(bool calculateForward)
{
    if (calculateForward) {
        // Update direction
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        forward = glm::normalize(front);
    }

    // Update other vectors
    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));

    // Update view matrix
    viewMatrix = glm::lookAt(pos, pos + forward, up);
}

} // namespace tmig