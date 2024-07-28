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
    std::shared_ptr<gl::VAO> vao = nullptr;

    /// @brief GMesh VBO
    std::shared_ptr<gl::VBO> vbo = nullptr;

    /// @brief GMesh EBO
    std::shared_ptr<gl::EBO> ebo = nullptr;

    /// @brief Method to setup OpenGL objects
    void setup();

private:
    /// @brief Whether setup was called (should be only once)
    bool _setupCalled;
};

} // namespace tmig