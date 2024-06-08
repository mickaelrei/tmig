#include <glad/glad.h>

#include "ebo.hpp"

EBO::EBO()
{
    glGenBuffers(1, &id);
}

EBO(const std::vector<unsigned int> &indices)
    : EBO{}
{
    bind();
    bufferData(indices.size() * sizeof(unsigned int), indices.data());
    unbind();
}

void EBO::bufferData(size_t size, const void *data)
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void EBO::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::destroy()
{
    glDeleteBuffers(1, &id);
}