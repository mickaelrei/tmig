#include "tmig/util/shapes.hpp"

namespace tmig::util {

void generateSphereMesh(
    const VertexGenerateCallback &vertexCallback,
    std::vector<unsigned int> &indices,
    const unsigned int resolution
) {
    const float step = 1.0f / (float)resolution;

    size_t i, j;
    for (i = 0; i <= resolution; ++i) {
        float v = 1.0f - (float)i * step;
        float theta = (float)i * step * M_PIf;

        for (j = 0; j < resolution; ++j) {
            float u;
            if (j <= resolution / 2) {
                u = (float)j * step * 2.0f;
            } else {
                u = 2.0f - (float)j * step * 2.0f;
            }

            float phi = (float)j * step * 2.0f * M_PIf;

            float x = std::sin(theta) * std::sin(phi);
            float y = std::cos(theta);
            float z = std::sin(theta) * std::cos(phi);

            vertexCallback(GeneralVertex{
                .position = glm::vec3{x, y, z} * 0.5f,
                .normal   = glm::vec3{x, y, z},
                .uv       = glm::vec2{u, v}
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

            indices.push_back(idx0);
            indices.push_back(idx1);
            indices.push_back(idx2);

            indices.push_back(idx2);
            indices.push_back(idx1);
            indices.push_back(idx3);
        }
    }
}

void generateCylinderMesh(
    const VertexGenerateCallback &vertexCallback,
    std::vector<unsigned int> &indices,
    const unsigned int resolution
) {
    // Add top and bottom vertex
    vertexCallback(GeneralVertex{
        .position = glm::vec3{0.0f, 0.5f, 0.0f},
        .normal   = glm::vec3{0.0f, 1.0f, 0.0f},
        .uv       = glm::vec2{0.5f, 1.0f},
    });
    vertexCallback(GeneralVertex{
        .position = glm::vec3{0.0f, -0.5f, 0.0f},
        .normal   = glm::vec3{0.0f, -1.0f, 0.0f},
        .uv       = glm::vec2{0.5f, 0.0f},
    });

    // Add bottom vertices
    float step = 1.0f / (float)resolution;
    for (size_t i = 0; i < resolution; ++i) {
        float u;
        if (i <= resolution / 2) {
            u = (float)i * step * 2.0f;
        } else {
            u = 2.0f - (float)i * step * 2.0f;
        }

        float angle = (float)i * step * 2.0f * M_PIf;
        float c = std::cos(angle);
        float s = std::sin(angle);

        // Vertex for top face
        vertexCallback(GeneralVertex{
            .position = glm::vec3{c * 0.5f, 0.5f, s * 0.5f},
            .normal   = glm::vec3{0.0f, 1.0f, 0.0f},
            .uv       = glm::vec2{u, 0.75f},
        });

        // Vertex for bottom face
        vertexCallback(GeneralVertex{
            .position = glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            .normal   = glm::vec3{0.0f, -1.0f, 0.0f},
            .uv       = glm::vec2{u, 0.25f},
        });

        // Vertices for side face
        // Top
        vertexCallback(GeneralVertex{
            .position = glm::vec3{c * 0.5f, 0.5f, s * 0.5f},
            .normal   = glm::vec3{c, 0.0f, s},
            .uv       = glm::vec2{u, 0.75f},
        });
        // Bottom
        vertexCallback(GeneralVertex{
            .position = glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            .normal   = glm::vec3{c, 0.0f, s},
            .uv       = glm::vec2{u, 0.25f},
        });
    }

    // Add indices
    for (size_t i = 2; i < (resolution * 4); i += 4) {
        // Top face
        indices.push_back(i);
        indices.push_back(0);
        indices.push_back(i == (resolution * 4 - 2) ? 2 : (i + 4));

        // Bottom face
        indices.push_back(1);
        indices.push_back(i + 1);
        indices.push_back(i == (resolution * 4 - 2) ? 3 : (i + 5));

        // Side face 1
        indices.push_back(i + 2);
        indices.push_back(i == (resolution * 4 - 2) ? 5 : (i + 7));
        indices.push_back(i + 3);

        // Side face 2
        indices.push_back(i + 2);
        indices.push_back(i == (resolution * 4 - 2) ? 4 : (i + 6));
        indices.push_back(i == (resolution * 4 - 2) ? 5 : (i + 7));
    }
}

void generateConeMesh(
    const VertexGenerateCallback &vertexCallback,
    std::vector<unsigned int> &indices,
    const unsigned int resolution
) {
    // Add top and bottom vertex
    vertexCallback(GeneralVertex{
        .position = glm::vec3{0.0f, 0.5f, 0.0f},
        .normal   = glm::vec3{0.0f, 1.0f, 0.0f},
        .uv       = glm::vec2{0.5f, 1.0f},
    });
    vertexCallback(GeneralVertex{
        .position = glm::vec3{0.0f, -0.5f, 0.0f},
        .normal   = glm::vec3{0.0f, -1.0f, 0.0f},
        .uv       = glm::vec2{0.5f, 0.0f},
    });

    // Add bottom vertices
    float step = 1.0f / (float)resolution;
    for (size_t i = 0; i < resolution; ++i) {
        float u;
        if (i <= resolution / 2) {
            u = (float)i * step * 2.0f;
        } else {
            u = 2.0f - (float)i * step * 2.0f;
        }

        float angle = (float)i * step * 2.0f * M_PIf;
        float c = std::cos(angle);
        float s = std::sin(angle);

        // Vertex for top face
        vertexCallback(GeneralVertex{
            .position = glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            .normal   = glm::vec3{c, 0.0f, s},
            .uv       = glm::vec2{u, 0.5f},
        });

        // Vertex for bottom face
        vertexCallback(GeneralVertex{
            .position = glm::vec3{c * 0.5f, -0.5f, s * 0.5f},
            .normal   = glm::vec3{0.0f, -1.0f, 0.0f},
            .uv       = glm::vec2{u, 0.5f},
        });
    }

    // Add indices
    for (size_t i = 2; i <= (resolution * 2); i += 2) {
        // Face connecting to top vertex
        indices.push_back(i);
        indices.push_back(0);
        indices.push_back(i == (resolution * 2) ? 2 : (i + 2));

        // Face connecting to bottom vertex
        indices.push_back(1);
        indices.push_back(i + 1);
        indices.push_back(i == (resolution * 2) ? 3 : (i + 3));
    }
}

void generateTorusMesh(
    const VertexGenerateCallback &vertexCallback,
    std::vector<unsigned int> &indices,
    const unsigned int resolution
) {
    // Add vertices
    float step = 1.0f / (float)resolution;
    size_t i, j;
    for (i = 0; i < resolution; ++i) {
        float u;
        if (i <= resolution / 2) {
            u = (float)i * step * 2.0f;
        } else {
            u = 2.0f - (float)i * step * 2.0f;
        }

        float angle0 = (float)i * step * 2.0f * M_PIf;
        float x0 = std::cos(angle0);
        float z0 = std::sin(angle0);

        for (j = 0; j < resolution; ++j) {
            float v;
            if (j <= resolution / 2) {
                v = (float)j * step * 2.0f;
            } else {
                v = 2.0f - (float)j * step * 2.0f;
            }
            v = 1.0f - v;

            float angle = (float)j * step * 2.0f * M_PIf;
            float c = std::cos(angle);
            float s = std::sin(angle);

            float x = x0 + s * x0 * 0.5f;
            float y = c * 0.5f;
            float z = z0 + s * z0 * 0.5f;

            vertexCallback(GeneralVertex{
                .position = glm::vec3{x, y, z},
                .normal   = glm::normalize(glm::vec3{s * x0, c, s * z0}),
                .uv       = glm::vec2{u, v},
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

            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx1);

            indices.push_back(idx2);
            indices.push_back(idx3);
            indices.push_back(idx1);
        }
    }
}

void generateBoxMesh(const VertexGenerateCallback &vertexCallback, std::vector<unsigned int> &indices) {
    std::vector<GeneralVertex> vertices = {
        // Front
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, 0.0f,  1.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, 0.0f,  1.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f,  0.5f}, .normal = glm::vec3{0.0f, 0.0f,  1.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f,  0.5f}, .normal = glm::vec3{0.0f, 0.0f,  1.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Back
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Left
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f,  0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Right
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{ 1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{ 1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{ 1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f,  0.5f}, .normal = glm::vec3{ 1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Top
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f,  0.5f}, .normal = glm::vec3{0.0f,  1.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f,  0.5f}, .normal = glm::vec3{0.0f,  1.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f,  1.0f, 0.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f,  1.0f, 0.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Bottom
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{0.0f, 1.0f}},
    };

    for (auto v : vertices) {
        vertexCallback(v);
    }

    indices = {
        // Front
        0, 1, 2,
        0, 2, 3,

        // Back
        4, 5, 6,
        4, 6, 7,

        // Left
        8, 9, 10,
        8, 10, 11,

        // Right
        12, 13, 14,
        12, 14, 15,

        // Top
        16, 17, 18,
        16, 18, 19,

        // Bottom
        20, 21, 22,
        20, 22, 23
    };
}

void generateWedgeMesh(const VertexGenerateCallback &vertexCallback, std::vector<unsigned int> &indices) {
    std::vector<GeneralVertex> vertices = {
        // Back
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.0f, -1.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Bottom
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, -1.0f, 0.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Top
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, 0.707f, 0.707f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{0.0f, 0.707f, 0.707f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.707f, 0.707f}, .uv = glm::vec2{1.0f, 1.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{0.0f, 0.707f, 0.707f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Left
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f, -0.5f,  0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{-0.5f,  0.5f, -0.5f}, .normal = glm::vec3{-1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 1.0f}},

        // Right
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f,  0.5f}, .normal = glm::vec3{1.0f, 0.0f, 0.0f}, .uv = glm::vec2{0.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f, -0.5f, -0.5f}, .normal = glm::vec3{1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 0.0f}},
        GeneralVertex{.position = glm::vec3{ 0.5f,  0.5f, -0.5f}, .normal = glm::vec3{1.0f, 0.0f, 0.0f}, .uv = glm::vec2{1.0f, 1.0f}},
    };

    for (auto v : vertices) {
        vertexCallback(v);
    }

    indices = {
        // Back
        0, 1, 2,
        0, 2, 3,

        // Bottom
        4, 5, 6,
        4, 6, 7,

        // Top
        8, 9, 10,
        8, 10, 11,

        // Left
        12, 13, 14,

        // Right
        15, 16, 17,
    };
}

} // namespace tmig::util