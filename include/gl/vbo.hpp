#pragma once

#include <vector>
#include <concepts>

#include "structs.hpp"

namespace tmig {

namespace gl {

class VBO
{
public:
    VBO();
    VBO(const std::vector<Vertex> &vertices);

    void bufferData(size_t size, const void *data) const;

    void bind() const;
    void unbind() const;
    void destroy() const;

    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig