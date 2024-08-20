#include <glm/glm.hpp>

#include "tmig/physics/collision/collision_test.hpp"

namespace tmig {

namespace physics {

namespace collision {

/// @brief Simplex class for holding points data
class Simplex {
public:
    Simplex()
        : points{{glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{0.0f}, glm::vec3{0.0f}}},
          _size{0} {}

    Simplex &operator=(std::initializer_list<glm::vec3> list) {
        for (auto v = list.begin(); v != list.end(); ++v) {
            points[std::distance(list.begin(), v)] = *v;
        }
        _size = list.size();

        return *this;
    }

    void push_front(const glm::vec3 &v) {
        points = {v, points[0], points[1], points[2]};
        _size = std::min(_size + 1, 4U);
    }

    glm::vec3 &operator[](unsigned int i) {
        return points[i];
    }

    unsigned int size() const {
        return _size;
    }

    const glm::vec3 *begin() const {
        return points.begin();
    }

    const glm::vec3 *end() const {
        return points.end() - (4U - _size);
    }

private:
    std::array<glm::vec3, 4> points;
    unsigned int _size;
};

glm::vec3 support(
    const Collider *colliderA,
    const Collider *colliderB,
    const glm::vec3 &direction
) {
    return colliderA->furthestPoint( direction)
         - colliderB->furthestPoint(-direction);
}

bool sameDirection(const glm::vec3 &v0, const glm::vec3 &v1) {
    return glm::dot(v0, v1) > 0.0f;
}

bool line(Simplex &points, glm::vec3 &direction) {
    auto a = points[0];
    auto b = points[1];

    auto ab = b - a;
    auto ao =   - a;

    if (sameDirection(ab, ao)) {
        direction = glm::cross(glm::cross(ab, ao), ab);
    } else {
        points = {a};
        direction = ao;
    }

    // A line can never cover a 3d point
    return false;
}

bool triangle(Simplex &points, glm::vec3 &direction) {
    auto a = points[0];
    auto b = points[1];
    auto c = points[2];

    auto ab = b - a;
    auto ac = c - a;
    auto ao =   - a;

    auto abc = glm::cross(ab, ac);

    if (sameDirection(glm::cross(abc, ac), ao)) {
        if (sameDirection(ac, ao)) {
            points = {a, c};
            direction = glm::cross(glm::cross(ac, ao), ac);
        } else {
            return line(points = {a, b}, direction);
        }
    } else {
        if (sameDirection(glm::cross(ab, abc), ao)) {
            return line(points = {a, b}, direction);
        } else {
            if (sameDirection(abc, ao)) {
                direction = abc;
            } else {
                points = {a, c, b};
                direction = -abc;
            }
        }
    }

    // A triangle can never cover a 3d point
    return false;
}

bool tetrahedron(Simplex &points, glm::vec3 &direction) {
    auto a = points[0];
    auto b = points[1];
    auto c = points[2];
    auto d = points[3];

    auto ab = b - a;
    auto ac = c - a;
    auto ad = d - a;
    auto ao =   - a;

    auto abc = glm::cross(ab, ac);
    auto acd = glm::cross(ac, ad);
    auto adb = glm::cross(ad, ab);

    if (sameDirection(abc, ao)) {
        return triangle(points = {a, b, c}, direction);
    }

    if (sameDirection(acd, ao)) {
        return triangle(points = {a, c, d}, direction);
    }

    if (sameDirection(adb, ao)) {
        return triangle(points = {a, d, b}, direction);
    }

    // If reached here, the tetrahedron covers the origin in 3d space
    return true;
}

bool nextSimplex(Simplex &points, glm::vec3 &direction) {
    switch (points.size()) {
        case 2: return line(points, direction);
        case 3: return triangle(points, direction);
        case 4: return tetrahedron(points, direction);
    }

    // Shouldn't be here...
    assert(false);
    return false;
}

bool hasCollision(Collider *colliderA, Collider *colliderB) {
    // Initial support point
    glm::vec3 direction{1.0f, 0.0f, 0.0f};
    glm::vec3 supportPoint = support(colliderA, colliderB, direction);

    // Create simplex
    Simplex points;
    points.push_front(supportPoint);

    // New direction is towards the origin
    direction = -supportPoint;

    while (true) {
        supportPoint = support(colliderA, colliderB, direction);

        // Check if new point if after origin
        if (glm::dot(supportPoint, direction) < 0.0f) {
            return false;
        }

        // Add new point to simplex
        points.push_front(supportPoint);

        if (nextSimplex(points, direction)) {
            return true;
        }
    }

    return false;
}

} // namespace collision

} // namespace physics

} // namespace tmig