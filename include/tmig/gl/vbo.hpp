#pragma once

#include <vector>
#include <memory>

#include "tmig/structs.hpp"
#include "tmig/gl/gl_object.hpp"

namespace tmig {

namespace gl {

/// @brief OpenGL Vertex Buffer Object (VBO) wrapper class
class VBO : public GLObject {
public:
    /// @brief Default constructor (does nothing)
    VBO() = default;

    /// @brief Creates a new VBO
    /// @param vertices list of vertices
    /// @return Shared pointer to new VBO
    static std::shared_ptr<VBO> create(const std::vector<Vertex> &vertices);

    /// @brief Set buffer data
    /// @param size buffer size
    /// @param data pointer to data
    void bufferData(size_t size, const void *data) const;

    /// @brief Bind object
    void bind() const;

    /// @brief Unbind object
    void unbind() const;

    /// @brief Destroy object
    void destroy() override;

private:
    /// @brief Constructor
    /// @param vertices list of vertices
    VBO(const std::vector<Vertex> &vertices);
};

} // namespace gl

} // namespace tmig