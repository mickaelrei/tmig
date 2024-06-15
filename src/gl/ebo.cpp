#include <glad/glad.h>

#include "gl/ebo.hpp"

namespace tmig {

namespace gl {

EBO::EBO() {}

EBO::EBO(const std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &id);
    bufferData(indices.size() * sizeof(unsigned int), indices.data());
}

void EBO::bufferData(size_t size, const void *data) const
{
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    unbind();
}

void EBO::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::destroy() const
{
    glDeleteBuffers(1, &id);
}

} // namespace gl

} // namespace tmig