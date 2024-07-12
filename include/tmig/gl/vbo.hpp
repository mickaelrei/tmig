#pragma once

#include <vector>
#include <concepts>

#include "tmig/structs.hpp"

namespace tmig {

namespace gl {

/// @brief OpenGL Vertex Buffer Object (VBO) wrapper class
class VBO {
public:
    /// @brief Default constructor (does nothing)
    VBO() = default;

    /// @brief Constructor
    /// @param vertices list of vertices
    VBO(const std::vector<Vertex> &vertices);

    /// @brief Set buffer data
    /// @param size buffer size
    /// @param data pointer to data
    void bufferData(size_t size, const void *data) const;

    /// @brief Bind object
    void bind() const;

    /// @brief Unbind object
    void unbind() const;

    /// @brief Destroy object
    void destroy() const;

private:
    /// @brief OpenGL object identifier
    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig