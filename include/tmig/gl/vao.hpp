#pragma once

#include "tmig/gl/vbo.hpp"

namespace tmig {

namespace gl {

/// @brief OpenGL Vertex Attribute Object (VAO) wrapper class
class VAO {
public:
    /// @brief Default constructor
    VAO();

    /// @brief Set vertex attribute
    /// @param vbo VBO to bind
    /// @param index attribute index
    /// @param size attribute data size
    /// @param type attribute data type
    /// @param stride attribute stride/size
    /// @param offset attribute offset
    void vertexAttrib(
        VBO &vbo,
        unsigned int index,
        int size,
        unsigned int type,
        int stride,
        const void *offset
    ) const;

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