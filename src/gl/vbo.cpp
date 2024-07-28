#include <iostream>

#include "glad/glad.h"

#include "tmig/gl/vbo.hpp"

namespace tmig {

namespace gl {

std::shared_ptr<VBO> VBO::create(const std::vector<Vertex> &vertices) {
    return std::shared_ptr<VBO>{new VBO{vertices}, Deleter{}};
}

VBO::VBO(const std::vector<Vertex> &vertices)
{
    glGenBuffers(1, &_id);
    printf("Created VBO id %d\n", _id);
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
    printf("destroy VBO id %d\n", _id);
    glDeleteVertexArrays(1, &_id);
}

} // namespace gl

} // namespace tmig