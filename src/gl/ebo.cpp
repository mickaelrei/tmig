#include <iostream>

#include "glad/glad.h"

#include "tmig/gl/ebo.hpp"

namespace tmig {

namespace gl {

std::shared_ptr<EBO> EBO::create(const std::vector<unsigned int> &indices) {
    return std::shared_ptr<EBO>{new EBO{indices}, Deleter{}};
}

EBO::EBO(const std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &_id);
    printf("Created EBO id %d\n", _id);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void EBO::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::destroy()
{
    printf("destroy EBO id %d\n", _id);
    glDeleteBuffers(1, &_id);
}

} // namespace gl

} // namespace tmig