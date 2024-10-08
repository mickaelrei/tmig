#include <iostream>

#include "glad/glad.h"

#include "tmig/utils.hpp"
#include "tmig/render/gl/vbo.hpp"

namespace tmig {

namespace render {

namespace gl {

std::shared_ptr<VBO> VBO::create(const std::vector<Vertex> &vertices) {
    return std::shared_ptr<VBO>{new VBO{vertices}, Deleter{}};
}

VBO::VBO(const std::vector<Vertex> &vertices)
{
    glGenBuffers(1, &_id);
    debug_print("Created VBO: %d\n", _id);
    bufferData(vertices.size() * sizeof(Vertex), vertices.data());
}

void VBO::bufferData(size_t size, const void *data) const
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    unbind();
}

void VBO::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VBO::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::destroy()
{
    debug_print("Destroyed VBO: %d\n", _id);
    glDeleteVertexArrays(1, &_id);
}

} // namespace gl

} // namespace render

} // namespace tmig