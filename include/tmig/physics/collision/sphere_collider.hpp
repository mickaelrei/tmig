#pragma once

#include "tmig/physics/collision/collider.hpp"

namespace tmig {

namespace physics {

namespace collision {
    
/// @brief Collider specific for spheres
class SphereCollider : public Collider {
public:
    /// @brief Default constructor
    SphereCollider() = default;

    /// @brief Constructor with sphere data
    /// @param center sphere center in world space
    /// @param radius sphere radius
    SphereCollider(const glm::vec3 &center, float radius);

    /// @brief Gets the furthest point in a direction, based on the sphere center and radius
    /// @param direction direction to check
    /// @return furthest point in direction
    glm::vec3 furthestPoint(const glm::vec3 &direction) const override;

private:
    /// @brief Sphere center in world space
    glm::vec3 center;

    /// @brief Sphere radius
    float radius;
};

} // namespace collision

} // namespace physics

} // namespace tmig