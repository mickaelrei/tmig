#include <glad/glad.h>

#include "gl/vao.hpp"

namespace tmig {

namespace gl {

VAO::VAO()
{
    glGenVertexArrays(1, &id);
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
    glBindVertexArray(id);
}

void VAO::unbind() const
{
    glBindVertexArray(0);
}

void VAO::destroy() const
{
    glDeleteVertexArrays(1, &id);
}

} // namespace gl

} // namespace tmig