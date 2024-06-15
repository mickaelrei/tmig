#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

#include "camera.hpp"
#include "entity.hpp"

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

void Camera::rotate(float rx, float ry, float dt)
{
    yaw += ry * rotationSpeed * dt;
    pitch += rx * rotationSpeed * dt;

    // Clamp to avoid gimball lock
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera::updateVectors()
{
    // Update direction
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = glm::normalize(front);

    // Update other vectors
    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, front));

    // Update view matrix
    viewMatrix = glm::lookAt(pos, pos + forward, up);
}

} // namespace tmig