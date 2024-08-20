#pragma once

#include <array>

#include "tmig/physics/collision/collider.hpp"
#include "tmig/physics/rigid_body.hpp"

namespace tmig {

namespace physics {

namespace collision {

/// @brief Support vector based on two colliders
/// @param colliderA first collider
/// @param colliderB second collider
/// @param direction direction vector
/// @return calculated support vector
glm::vec3 support(
    const Collider *colliderA,
    const Collider *colliderB,
    const glm::vec3 &direction
);

/// @brief Checks if two colliders are colliding
/// @param colliderA first collider
/// @param colliderB second collider
/// @return boolean telling whether there is a collision
bool hasCollision(Collider *colliderA, Collider *colliderB);

} // namespace collision

} // namespace physics

} // namespace tmig