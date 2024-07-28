#include <iostream>

#include "glad/glad.h"

#include "tmig/gl/vao.hpp"

namespace tmig {

namespace gl {

std::shared_ptr<VAO> VAO::create() {
    return std::shared_ptr<VAO>{new VAO{}, Deleter{}};
}

VAO::VAO()
{
    glGenVertexArrays(1, &_id);
    printf("Created VAO id %d\n", _id);
}

void VAO::vertexAttrib(
    VBO &vbo,
    unsigned int index, int size, unsigned int type,
    int stride, const void *offset) const
{
    bind();
    vbo.bind();

    // Enable vertex attribute
    glEnableVertexAttribArray(index);
    // Set information on vertex attribute
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, offset);

    vbo.unbind();
    unbind();
}

void VAO::bind() const
{
    glBindVertexArray(_id);
}

void VAO::unbind() const
{
    glBindVertexArray(0);
}

void VAO::destroy()
{
    printf("destroy VAO id %d\n", _id);
    glDeleteVertexArrays(1, &_id);
}

} // namespace gl

} // namespace tmig