#pragma once

#include <memory>

#include "tmig/render/gl/vbo.hpp"

#include "tmig/render/gl/gl_object.hpp"

namespace tmig {

namespace render {

namespace gl {

/// @brief OpenGL Vertex Attribute Object (VAO) wrapper class
class VAO : public GLObject {
public:
    /// @brief Default constructor
    VAO();

    /// @brief Creates a new VAO
    /// @return Shared pointer to new VAO
    static std::shared_ptr<VAO> create();

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
    void destroy() override;
};

} // namespace gl

} // namespace render

} // namespace tmig