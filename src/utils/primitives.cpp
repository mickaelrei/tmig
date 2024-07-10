#include "tmig/utils/primitives.hpp"
#include "tmig/utils/shaders.hpp"

namespace tmig {

namespace utils {

Mesh sphereMesh() {
    Mesh mesh;

    // Sphere resolution
    const size_t resolution = 20;

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

            mesh.vertices.push_back(Vertex{glm::vec3{x, y, z} * 0.5f, glm::vec2{texU, texV}, glm::vec3{x, y, z}});
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

Mesh cylinderMesh() {
    Mesh mesh;

    // Add top and bottom vertex
    mesh.vertices = {
        // Top
        Vertex{
            glm::vec3{0.0f, 0.5f, 0.0f},
            glm::vec2{0.5f, 1.0f},
            glm::vec3{0.0f, 1.0f, 0.0f}
        },
        // Bottom
        Vertex{
            glm::vec3{0.0f, -0.5f, 0.0f},
            glm::vec2{0.5f, 0.0f},
            glm::vec3{0.0f, -1.0f, 0.0f}
        }
    };

    // Add bottom vertices
    const size_t resolution = 20;
    float step = 1.0f / (float)resolution;
    for (size_t i = 0; i < resolution; ++i) {
        float texU;
        if (i <= resolution / 2)
        {
            texU = (float)i * step * 2.0f;
        }
        else
        {
            texU = 2.0f - (float)i * step * 2.0f;
        }
        float angle = (float)i * step * 2.0f * M_PIf;
        float c = std::cos(angle);
        float s = std::sin(angle);

        // Vertex for top face
        mesh.vertices.push_back(Vertex{
            glm::vec3{c * 0.5f, 0.5f, s * 0.5f},
            glm::vec2{texU, 0.75f},
            glm::vec3{0.0f, 1.0f, 0.0f}
        });

        // Vertex for bottom face
        mesh.vertices.push_back(Vertex{
            glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            glm::vec2{texU, 0.25f},
            glm::vec3{0.0f, -1.0f, 0.0f}
        });

        // Vertices for side face
        // Top
        mesh.vertices.push_back(Vertex{
            glm::vec3{c * 0.5f, 0.5f, s * 0.5f},
            glm::vec2{texU, 0.75f},
            glm::vec3{c, 0.0f, s}
        });
        // Bottom
        mesh.vertices.push_back(Vertex{
            glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            glm::vec2{texU, 0.25f},
            glm::vec3{c, 0.0f, s}
        });
    }

    // Add indices
    for (size_t i = 2; i < (resolution * 4); i += 4) {
        // Top face
        mesh.indices.push_back(i);
        mesh.indices.push_back(0);
        mesh.indices.push_back(i == (resolution * 4 - 2) ? 2 : (i + 4));

        // Bottom face
        mesh.indices.push_back(1);
        mesh.indices.push_back(i + 1);
        mesh.indices.push_back(i == (resolution * 4 - 2) ? 3 : (i + 5));

        // Side face 1
        mesh.indices.push_back(i + 2);
        mesh.indices.push_back(i == (resolution * 4 - 2) ? 5 : (i + 7));
        mesh.indices.push_back(i + 3);

        // Side face 2
        mesh.indices.push_back(i + 2);
        mesh.indices.push_back(i == (resolution * 4 - 2) ? 4 : (i + 6));
        mesh.indices.push_back(i == (resolution * 4 - 2) ? 5 : (i + 7));
    }

    return mesh;
}

Mesh coneMesh() {
    Mesh mesh;

    // Add top and bottom vertex
    mesh.vertices = {
        // Top
        Vertex{
            glm::vec3{0.0f, 0.5f, 0.0f},
            glm::vec2{0.5f, 1.0f},
            glm::vec3{0.0f, 1.0f, 0.0f}
        },
        // Bottom
        Vertex{
            glm::vec3{0.0f, -0.5f, 0.0f},
            glm::vec2{0.5f, 0.0f},
            glm::vec3{0.0f, -1.0f, 0.0f}
        }
    };

    // Add bottom vertices
    const size_t resolution = 20;
    float step = 1.0f / (float)resolution;
    for (size_t i = 0; i < resolution; ++i) {
        float texU;
        if (i <= resolution / 2)
        {
            texU = (float)i * step * 2.0f;
        }
        else
        {
            texU = 2.0f - (float)i * step * 2.0f;
        }
        float angle = (float)i * step * 2.0f * M_PIf;
        float c = std::cos(angle);
        float s = std::sin(angle);

        // Vertex for top face
        mesh.vertices.push_back(Vertex{
            glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            glm::vec2{texU, 0.5f},
            glm::vec3{c, 0.0f, s}
        });

        // Vertex for bottom face
        mesh.vertices.push_back(Vertex{
            glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            glm::vec2{texU, 0.5f},
            glm::vec3{0.0f, -1.0f, 0.0f}
        });
    }

    // Add indices
    for (size_t i = 2; i <= (resolution * 2); i += 2) {
        // Face connecting to top vertex
        mesh.indices.push_back(i);
        mesh.indices.push_back(0);
        mesh.indices.push_back(i == (resolution * 2) ? 2 : (i + 2));

        // Face connecting to bottom vertex
        mesh.indices.push_back(1);
        mesh.indices.push_back(i + 1);
        mesh.indices.push_back(i == (resolution * 2) ? 3 : (i + 3));
    }

    return mesh;
}

} // namespace utils

} // namespace tmig