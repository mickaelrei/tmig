#include "tmig/utils/primitives.hpp"

namespace tmig {

namespace utils {

Mesh sphereMesh(const size_t resolution) {
    Mesh mesh;

    const float step = 1.0f / (float)resolution;

    size_t i, j;
    for (i = 0; i <= resolution; ++i) {
        float texV = 1.0f - (float)i * step;
        float theta = (float)i * step * M_PIf;
        for (j = 0; j < resolution; ++j) {
            float texU;
            if (j <= resolution / 2)
            {
                texU = (float)j * step * 2.0f;
            }
            else
            {
                texU = 2.0f - (float)j * step * 2.0f;
            }

            float phi = (float)j * step * 2.0f * M_PIf;

            float x = std::sin(theta) * std::sin(phi);
            float y = std::cos(theta);
            float z = std::sin(theta) * std::cos(phi);

            mesh.vertices.push_back(Vertex{
                glm::vec3{x, y, z} * 0.5f,
                glm::vec2{texU, texV},
                glm::vec3{x, y, z}
            });
        }
    }

    // Add indices
    for (i = 0; i < resolution; ++i) {
        for (j = 0; j < resolution; ++j) {
            size_t _j = (j + 1) % resolution;

            size_t idx0 = (i + 0) * resolution +  j;
            size_t idx1 = (i + 1) * resolution +  j;
            size_t idx2 = (i + 0) * resolution + _j;
            size_t idx3 = (i + 1) * resolution + _j;

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

Mesh cylinderMesh(const size_t resolution) {
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

Mesh coneMesh(const size_t resolution) {
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

Mesh torusMesh(const size_t resolution) {
    Mesh mesh;

    // Add vertices
    float step = 1.0f / (float)resolution;
    size_t i, j;
    for (i = 0; i < resolution; ++i) {
        float texU;
        if (i <= resolution / 2)
        {
            texU = (float)i * step * 2.0f;
        }
        else
        {
            texU = 2.0f - (float)i * step * 2.0f;
        }

        float angle0 = (float)i * step * 2.0f * M_PIf;
        float x0 = std::cos(angle0);
        float z0 = std::sin(angle0);

        for (j = 0; j < resolution; ++j) {
            float texV;
            if (j <= resolution / 2)
            {
                texV = (float)j * step * 2.0f;
            }
            else
            {
                texV = 2.0f - (float)j * step * 2.0f;
            }
            texV = 1.0f - texV;

            float angle = (float)j * step * 2.0f * M_PIf;
            float c = std::cos(angle);
            float s = std::sin(angle);

            float x = x0 + s * x0 * 0.5f;
            float y = c * 0.5f;
            float z = z0 + s * z0 * 0.5f;

            mesh.vertices.push_back(Vertex{
                glm::vec3{x, y, z},
                glm::vec2{texU, texV},
                glm::normalize(glm::vec3{s * x0, c, s * z0})
            });
        }
    }

    for (i = 0; i < resolution; ++i) {
        for (j = 0; j < resolution; ++j) {
            size_t _i = (i + 1) % resolution;
            size_t _j = (j + 1) % resolution;

            size_t idx0 =  i * resolution +  j;
            size_t idx1 =  i * resolution + _j;
            size_t idx2 = _i * resolution +  j;
            size_t idx3 = _i * resolution + _j;

            mesh.indices.push_back(idx0);
            mesh.indices.push_back(idx2);
            mesh.indices.push_back(idx1);

            mesh.indices.push_back(idx2);
            mesh.indices.push_back(idx3);
            mesh.indices.push_back(idx1);
        }
    }

    return mesh;
}

} // namespace utils

} // namespace tmig