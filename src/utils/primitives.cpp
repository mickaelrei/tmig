#include "utils/primitives.hpp"

#include "utils/shaders.hpp"

namespace tmig {

namespace utils {

Mesh sphereMesh() {
    Mesh mesh;

    // Sphere resolution
    const size_t resolution = 50;

    // Add vertices
    const float thetaStep = 1.0f / (float)resolution;
    const float phiStep = 1.0f / (float)resolution;

    size_t i, j;
    for (i = 0; i <= resolution; ++i) {
        float texV = 1.0f - (float)i * thetaStep;
        float theta = (float)i * thetaStep * M_PIf;
        for (j = 0; j < resolution; ++j) {
            float texU;
            if (j <= resolution / 2)
            {
                texU = (float)j * phiStep * 2.0f;
            }
            else
            {
                texU = 2.0f - (float)j * phiStep * 2.0f;
            }

            float phi = (float)j * phiStep * 2.0f * M_PIf;

            float x = std::sin(theta) * std::sin(phi);
            float y = std::cos(theta);
            float z = std::sin(theta) * std::cos(phi);

            mesh.vertices.push_back(Vertex{glm::vec3{x, y, z}, glm::vec2{texU, texV}});
        }
    }

    // Add indices
    for (i = 0; i < resolution; ++i) {
        for (j = 0; j < resolution; ++j) {
            size_t idx0 = (i + 0) * resolution + (j + 0);
            size_t idx1 = (i + 1) * resolution + (j + 0);
            size_t idx2 = (i + 0) * resolution + (j + 1);
            size_t idx3 = (i + 1) * resolution + (j + 1);

            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx1);
            mesh.indices.push_back(idx2);

            mesh.indices.push_back(idx2);
            mesh.indices.push_back(idx1);
            mesh.indices.push_back(idx3);
        }
    }

    return mesh;
}

} // namespace utils

} // namespace tmig