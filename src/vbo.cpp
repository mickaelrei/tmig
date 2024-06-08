#include <glad/glad.h>

#include "vbo.hpp"

VBO::VBO()
{
    glGenBuffers(1, &id);
}

VBO::VBO(const std::vector<Vertex> &vertices)
    : VBO{}
{
    bind();
    bufferData(vertices.size() * sizeof(Vertex), vertices.data());
    unbind();
}

void VBO::bufferData(size_t size, const void *data)
{
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::destroy()
{
    glDeleteVertexArrays(1, &id);
}
