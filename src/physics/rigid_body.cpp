#include "glad/glad.h"

#include "tmig/physics/collision/collider.hpp"
#include "tmig/physics/rigid_body.hpp"

#include "tmig/render/utils/primitives_gmesh.hpp"

namespace tmig {

namespace physics {

RigidBody::RigidBody(const glm::vec3 &scale)
    : Entity::Entity{render::utils::boxGMesh()} {
    setScale(scale);

    // Create rotation axes entities
    auto s = 0.1f;
    axisX = std::make_shared<Entity>(render::utils::boxGMesh());
    axisX->setColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    axisX->setScale(glm::vec3{1.0f, s, s});

    axisY = std::make_shared<Entity>(render::utils::boxGMesh());
    axisY->setColor(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    axisY->setScale(glm::vec3{s, 1.0f, s});

    axisZ = std::make_shared<Entity>(render::utils::boxGMesh());
    axisZ->setColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
    axisZ->setScale(glm::vec3{s, s, 1.0f});
}

void RigidBody::update(float dt) {
    // Update auxiliary variables
    R = glm::mat3{rot};
    iInv = R * iBodyInv * glm::transpose(R);
    vel = linearMomentum / mass;
    omega = iInv * angularMomentum;

    // Update position
    setPosition(_position + vel);

    // Update rotation
    glm::quat qdt = 0.5f * (omega * rot);
    rot = glm::normalize(rot * qdt);
    Entity::setRotation(glm::mat4{rot});

    // Update linear and angular momentum
    linearMomentum += totalForce * dt;
    angularMomentum += totalTorque * dt;

    // Apply drag to reduce momentum
    linearMomentum *= 1.0f - linearDrag * dt;
    angularMomentum *= 1.0f - angularDrag * dt;

    // Reset force and torque
    totalForce = glm::vec3{0.0f};
    totalTorque = glm::vec3{0.0f};
}

void RigidBody::setRotation(const glm::mat4 &rotation) {
    rot = glm::quat{rotation};
    Entity::setRotation(rotation);
}

void RigidBody::setScale(const glm::vec3 &scale) {
    // Set scale variable
    _scale = scale;
    updateModelMatrix();
    auto scale2 = glm::pow(scale, glm::vec3{2.0f});

    // Set constants based on scale
    mass = scale.x * scale.y * scale.z;
    auto iBody = mass / 12.0f * glm::mat3{
        scale2.y + scale2.z, 0.0f, 0.0f,
        0.0f, scale2.x + scale2.z, 0.0f,
        0.0f, 0.0f, scale2.x + scale2.y
    };
    iBodyInv = glm::inverse(iBody);
}

void RigidBody::draw(const tmig::render::gl::Shader &shader) const {
    Entity::draw(shader);

    // Correct position and rotation for each axis entity
    auto rotation = getRotation();
    axisX->setRotation(rotation);
    axisY->setRotation(rotation);
    axisZ->setRotation(rotation);

    auto pos = getPosition();
    axisX->setPosition(pos - glm::mat3{rotation} * glm::vec3{-0.5f,  0.0f,  0.0f});
    axisY->setPosition(pos - glm::mat3{rotation} * glm::vec3{ 0.0f, -0.5f,  0.0f});
    axisZ->setPosition(pos - glm::mat3{rotation} * glm::vec3{ 0.0f,  0.0f, -0.5f});

    glDisable(GL_DEPTH_TEST);
    // Draw rotation axes
    axisX->draw(shader);
    axisY->draw(shader);
    axisZ->draw(shader);
    glEnable(GL_DEPTH_TEST);
}

void RigidBody::applyForce(const glm::vec3 &force) {
    totalForce += force;
}

void RigidBody::applyTorque(const glm::vec3 &torque) {
    totalTorque += torque;
}

void RigidBody::applyRelativeForce(const glm::vec3 &force) {
    // Convert [force] vector to relative coordinates before applying
    glm::vec3 relativeForce = glm::mat3{getRotation()} * force;
    applyForce(relativeForce);
}

void RigidBody::applyRelativeTorque(const glm::vec3 &torque) {
    // Convert [torque] vector to relative coordinates before applying
    glm::vec3 relativeTorque = glm::mat3{getRotation()} * torque;
    applyTorque(relativeTorque);
}

void RigidBody::applyForceAtPosition(const glm::vec3 &force, const glm::vec3 &pos) {
    // Apply force at center of body normally
    applyForce(force);

    // Calculate torque based on position and center of mass
    glm::vec3 torque = glm::cross(pos - getPosition(), force);
    applyTorque(torque);
}

glm::vec3 RigidBody::pointToLocalSpace(const glm::vec3 &worldPoint) const {
    glm::vec3 point = worldPoint - getPosition();
    return glm::inverse(glm::mat3{getRotation()}) * point;
}

glm::vec3 RigidBody::pointToWorldSpace(const glm::vec3 &localPoint) const {
    glm::vec3 point = glm::mat3{getRotation()} * localPoint;
    return point + getPosition();
}

glm::vec3 RigidBody::vectorToLocalSpace(const glm::vec3 &worldVector) const {
    return glm::inverse(glm::mat3{getRotation()}) * worldVector;
}

glm::vec3 RigidBody::vectorToWorldSpace(const glm::vec3 &localVector) const {
    return glm::mat3{getRotation()} * localVector;
}

float RigidBody::getMass() const {
    return mass;
}

void RigidBody::setLinearDrag(float drag) {
    linearDrag = std::max(0.0f, drag);
}

float RigidBody::getLinearDrag() const {
    return linearDrag;
}

void RigidBody::setAngularDrag(float drag) {
    angularDrag = std::max(0.0f, drag);
}

float RigidBody::getAngularDrag() const {
    return angularDrag;
}

} // namespace physics

} // namespace tmig