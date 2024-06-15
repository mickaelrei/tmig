#pragma once

#include "vbo.hpp"

namespace tmig {

namespace gl {

class VAO
{
public:
    VAO();

    void vertexAttrib(
        VBO &vbo,
        unsigned int index,
        int size,
        unsigned int type,
        int stride,
        const void *offset
    ) const;

    void bind() const;
    void unbind() const;
    void destroy() const;

    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig