#include "tmig/physics/collision/mesh_collider.hpp"

namespace tmig {

namespace physics {

namespace collision {

MeshCollider::MeshCollider(const Mesh &mesh, const glm::mat4 *modelMatrix)
    : modelMatrix{modelMatrix} {
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

    // Get model translation
    glm::vec3 translation = (*modelMatrix)[3];

    // Get point with maximum dot
    const size_t size = vertices.size();
    for (size_t i = 0; i < size; ++i) {
        // Convert to world space
        glm::vec3 pos = (*modelMatrix) * glm::vec4{vertices[i], 0.0f};
        pos += translation;

        // Check new dot
        float dot = glm::dot(normDir, pos);
        if (dot > maxDot) {
            maxDot = dot;
            furthest = pos;
        }
    }

    return furthest;
}

glm::vec3 MeshCollider::getCenter() const {
    return (*modelMatrix)[3];
}

} // namespace collision

} // namespace physics

} // namespace tmig