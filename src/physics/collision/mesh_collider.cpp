#include "tmig/physics/collision/mesh_collider.hpp"

namespace tmig {

namespace physics {

namespace collision {

MeshCollider::MeshCollider(const Mesh &mesh) {
    // Allocate enough space
    const size_t size = mesh.vertices.size();
    vertices.resize(size);

    // Populate vertices vector
    for (size_t i = 0; i < size; ++i) {
        vertices.push_back(mesh.vertices[i].pos);
    }
}

glm::vec3 MeshCollider::furthestPoint(const glm::vec3 &direction) const {
    // Check if it's a zero vector
    if (direction == glm::vec3{0.0f}) {
        return glm::vec3{0.0f};
    }

    // Normalize direction
    const glm::vec3 normDir = glm::normalize(direction);

    // Save furthest point
    glm::vec3 furthest;
    float maxDot = -std::numeric_limits<float>::max();

    // Get point with maximum dot
    const size_t size = vertices.size();
    for (size_t i = 0; i < size; ++i) {
        glm::vec3 pos = vertices[i];
        float dot = glm::dot(normDir, pos);

        if (dot > maxDot) {
            maxDot = dot;
            furthest = pos;
        }
    }

    return furthest;
}

} // namespace collision

} // namespace physics

} // namespace tmig