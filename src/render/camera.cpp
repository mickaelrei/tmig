#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tmig/render/camera.hpp"

namespace tmig::render {

Camera::Camera() {
    updateViewMatrix();
}

Camera::Camera(const glm::vec3& position) : pos(position) {
    updateViewMatrix();
}

void Camera::setPosition(const glm::vec3& position) {
    pos = position;
    updateViewMatrix();
}

glm::vec3 Camera::getPosition() const {
    return pos;
}

void Camera::move(const glm::vec3& offset) {
    pos += offset;
    updateViewMatrix();
}

void Camera::moveRelative(const glm::vec3& offset) {
    pos += rotation * offset;
    updateViewMatrix();
}

void Camera::rotate(float pitch, float yaw, float roll) {
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3{1.0f, 0.0f, 0.0f});
    glm::quat qYaw   = glm::angleAxis(yaw,   glm::vec3{0.0f, 1.0f, 0.0f});
    glm::quat qRoll  = glm::angleAxis(roll,  glm::vec3{0.0f, 0.0f, 1.0f});
    rotation = glm::normalize(qYaw * qPitch * qRoll * rotation);
    updateViewMatrix();
}

void Camera::rotate(const glm::quat& rot) {
    rotation = glm::normalize(rot * rotation);
    updateViewMatrix();
}

void Camera::setRotation(const glm::quat& rot) {
    rotation = glm::normalize(rot);
    updateViewMatrix();
}

glm::quat Camera::getRotation() const {
    return rotation;
}

glm::vec3 Camera::getForward() const {
    return glm::rotate(rotation, glm::vec3{0.0f, 0.0f, -1.0f});
}

glm::vec3 Camera::getRight() const {
    return glm::rotate(rotation, glm::vec3{1.0f, 0.0f, 0.0f});
}

glm::vec3 Camera::getUp() const {
    return glm::rotate(rotation, glm::vec3{0.0f, 1.0f, 0.0f});
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

void Camera::lookAt(const glm::vec3& target, const glm::vec3& up) {
    if (glm::length2(pos - target) > 0.0001f) {
        glm::vec3 dir = glm::normalize(target - pos);
        rotation = glm::quatLookAt(dir, up);
    }
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    // viewMatrix = glm::lookAt(pos, pos + getForward(), getUp());
    viewMatrix = glm::translate(glm::mat4_cast(glm::conjugate(rotation)), -pos);
}

} // namespace tmig::render