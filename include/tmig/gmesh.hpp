#pragma once

#include <vector>

#include "tmig/gl/vao.hpp"
#include "tmig/gl/vbo.hpp"
#include "tmig/gl/ebo.hpp"
#include "tmig/mesh.hpp"

namespace tmig {

/// @brief Struct to represent a mesh with OpenGL objects for drawing
struct GMesh : public Mesh {
    /// @brief Default constructor
    GMesh() = default;

    /// @brief Constructor
    /// @param mesh mesh
    GMesh(const Mesh &mesh);

    /// @brief GMesh VAO
    gl::VAO vao;

    /// @brief GMesh VBO
    gl::VBO vbo;

    /// @brief GMesh EBO
    gl::EBO ebo;

    /// @brief Method to setup OpenGL objects
    void setup();

private:
    /// @brief Whether setup was called (should be only once)
    bool _setupCalled;
};

} // namespace tmig