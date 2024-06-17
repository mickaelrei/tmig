#include "glad/glad.h"

#include "tmig/gl/vbo.hpp"

namespace tmig {

namespace gl {

VBO::VBO() {}

VBO::VBO(const std::vector<Vertex> &vertices)
{
    glGenBuffers(1, &id);
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
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::destroy() const
{
    glDeleteVertexArrays(1, &id);
}

} // namespace gl

} // namespace tmig