#include <utils/sphere.hpp>

Sphere::Sphere(const glm::vec3 &pos, const float radius)
{
    // TODO: Generate vertices and indices
    vertices.clear();
    indices.clear();

    const size_t resolution = 20;
}

float Sphere::getRadius() const
{
    // Scale vector will always be the same on x, y and z
    return getScale().x;
}

void Sphere::setRadius(const float radius)
{
    setScale(glm::vec3{radius});
}
