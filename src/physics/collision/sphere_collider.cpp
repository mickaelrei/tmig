#include "tmig/physics/collision/sphere_collider.hpp"

namespace tmig {

namespace physics {

namespace collision {

SphereCollider::SphereCollider(const glm::vec3 *center, const float *radius)
    : center{center},
      radius{radius} {}


glm::vec3 SphereCollider::furthestPoint(const glm::vec3 &direction) const {
    // Furthest point in a direction on a sphere is just the direction
    // scaled by the radius and offset by the center
    return direction * (*radius) + (*center);
}

} // namespace collision

} // namespace physics

} // namespace tmig