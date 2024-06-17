#pragma once

#include "tmig/entity.hpp"

namespace tmig {

namespace utils {

Mesh sphereMesh();

static const Mesh boxMesh = {
    {
        // Front
        Vertex{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 1.0f}},

        // Back
        Vertex{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 1.0f}},

        // Left
        Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 1.0f}},

        // Right
        Vertex{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 1.0f}},

        // Top
        Vertex{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 1.0f}},

        // Bottom
        Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec2{1.0f, 0.0f}},
        Vertex{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec2{1.0f, 1.0f}},
        Vertex{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 1.0f}},
    },
    {
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
    }
};

} // namespace utils

} // namespace tmig