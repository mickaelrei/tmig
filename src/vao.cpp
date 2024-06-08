#include <glad/glad.h>

#include "vao.hpp"

VAO::VAO()
{
    glGenVertexArrays(1, &id);
}

void VAO::vertexAttrib(
    VBO &vbo,
    unsigned int index, int size, unsigned int type,
    int stride, const void *offset)
{
    bind();
    vbo.bind();

    // Set information on vertex attribute
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, offset);
    // Enable vertex attribute
    glEnableVertexAttribArray(index);

    vbo.unbind();
    unbind();
}

void VAO::bind()
{
    glBindVertexArray(id);
}

void VAO::unbind()
{
    glBindVertexArray(0);
}

void VAO::destroy()
{
    glDeleteVertexArrays(1, &id);
}