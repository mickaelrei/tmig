#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace tmig {

namespace physics {

namespace collision {

/// @brief Abstract class defining a collider used in collision tests
class Collider {
public:
    /// @brief Destructor
    virtual ~Collider() {};

    /// @brief Gets the furthest point in world space for a given direction
    /// @param direction direction to check
    /// @return furthest point in direction
    virtual glm::vec3 furthestPoint(const glm::vec3 &direction) const = 0;

    /// @brief Gets the collider center in world space
    /// @return center point
    virtual glm::vec3 getCenter() const = 0;
};

} // namespace collision

} // namespace physics

} // namespace tmig