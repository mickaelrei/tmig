#pragma once

#include <glm/gtx/string_cast.hpp>

#include "tmig/render/entity.hpp"

namespace tmig {

namespace physics {

/// @brief Rigid Body class for physics simulation
class RigidBody : public render::Entity {
public:
    /// @brief Default constructor (does nothing)
    RigidBody() = default;

    /// @brief Constructor with scale
    /// @param scale initial body scale
    RigidBody(const glm::vec3 &scale);

    /// @brief Updates body data
    /// @param dt time between frames in secons
    void update(float dt);

    /// @brief Sets new rotation for rigid body
    /// @param rotation new rotation matrix
    void setRotation(const glm::mat4 &rotation) override;

    /// @brief Sets new scale for rigid body (recalculates data)
    /// @param scale new scale vector
    void setScale(const glm::vec3 &scale) override;

    /// @brief Draws rigid body and axis for visualization
    /// @param shader shader to use for rendering
    void draw(const tmig::render::gl::Shader &shader) const override;

    /// @brief Applies a force at the center of the body
    /// @param force force vector in world space
    void applyForce(const glm::vec3 &force);

    /// @brief Applies a rotation torque at the center of the body
    /// @param torque torque vector in world space
    void applyTorque(const glm::vec3 &torque);

    /// @brief Applies a force relative to the body's coordinate space
    /// @param force force vector in local space
    void applyRelativeForce(const glm::vec3 &force);

    /// @brief Applies a rotation torque relative to the body's coordinate space
    /// @param torque torque vector in local space
    void applyRelativeTorque(const glm::vec3 &torque);

    /// @brief Applies a force at a position. This will apply both a force and a torque on the body
    /// @param force force vector in world space
    /// @param pos position in world space
    void applyForceAtPosition(const glm::vec3 &force, const glm::vec3 &pos);

    /// @brief Converts a point in world space to local space
    /// @param worldPoint point in world space
    /// @return point in local space
    glm::vec3 pointToLocalSpace(const glm::vec3 &worldPoint) const;

    /// @brief Converts a point in local space to world space
    /// @param worldPoint point in local space
    /// @return point in world space
    glm::vec3 pointToWorldSpace(const glm::vec3 &localPoint) const;

    /// @brief Converts a vector in world space to local space
    /// @param worldVector vector in world space
    /// @return vector in local space
    glm::vec3 vectorToLocalSpace(const glm::vec3 &worldVector) const;

    /// @brief Converts a vector in local space to world space
    /// @param localVector vector in local space
    /// @return vector in world space
    glm::vec3 vectorToWorldSpace(const glm::vec3 &localVector) const;

    /// @brief Gets object mass
    /// @return mass
    float getMass() const;

    /// @brief Sets new linear drag
    /// @param drag linear drag
    void setLinearDrag(float drag);

    /// @brief Gets current linear drag
    /// @return linear drag
    float getLinearDrag() const;

    /// @brief Sets new angular drag
    /// @param drag angular drag
    void setAngularDrag(float drag);

    /// @brief Gets current angular drag
    /// @return angular drag
    float getAngularDrag() const;

private:
    /// -------------------------------------------------------------- ///
    ///                        MISCELLANEOUS                           ///
    /// -------------------------------------------------------------- ///

    /// @brief Entity for visualization of local x-axis
    std::shared_ptr<Entity> axisX;

    /// @brief Entity for visualization of local y-axis
    std::shared_ptr<Entity> axisY;

    /// @brief Entity for visualization of local z-axis
    std::shared_ptr<Entity> axisZ;

    /// -------------------------------------------------------------- ///
    ///                         CONSTANTS                              ///
    /// -------------------------------------------------------------- ///

    /// @brief Body mass
    float mass;

    /// @brief Inverse of body inertia tensor
    /// @link https://www.cs.cmu.edu/~baraff/sigcourse/notesd1.pdf
    glm::mat3 iBodyInv;

    /// -------------------------------------------------------------- ///
    ///                         PROPERTIES                             ///
    /// -------------------------------------------------------------- ///

    /// @brief How much the object's linear momentum slows down over time
    float linearDrag = 1.0f;

    /// @brief How much the object's angular momentum slows down over time
    float angularDrag = 0.5f;

    /// -------------------------------------------------------------- ///
    ///                        CURRENT STATE                           ///
    /// -------------------------------------------------------------- ///

    /// @brief Rotation quaternion
    glm::quat rot = {1.0f, glm::vec3{0.0f}};

    /// @brief Linear momentum (change in position)
    glm::vec3 linearMomentum = glm::vec3{0.0f};

    /// @brief Angular momentum (change in rotation)
    glm::vec3 angularMomentum = glm::vec3{0.0f};

    /// -------------------------------------------------------------- ///
    ///                          AUXILIARY                             ///
    /// -------------------------------------------------------------- ///

    /// @brief Inverse of inertia tensor at current simulation step
    glm::mat3 iInv = glm::mat3{1.0f};

    /// @brief Cached rotation matrix at current simulation step
    glm::mat3 R = glm::mat3{1.0f};

    /// @brief Linear velocity at current simulation step
    glm::vec3 vel = glm::vec3{0.0f};

    /// @brief Angular velocity at current simulation step
    glm::vec3 omega = glm::vec3{0.0f};

    /// -------------------------------------------------------------- ///
    ///                   EXTERNAL INTERACTION                         ///
    /// -------------------------------------------------------------- ///

    /// @brief Total force accumulated (such as gravity and wind) at current simulation step
    glm::vec3 totalForce = glm::vec3{0.0f};

    /// @brief Total torque accumulated (such as collision) at current simulation step
    glm::vec3 totalTorque = glm::vec3{0.0f};
};

} // namespace physics

} // namespace tmig