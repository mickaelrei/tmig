#pragma once

#include "vbo.hpp"

class VAO
{
public:
    VAO();

    void vertexAttrib(
        VBO &vbo,
        unsigned int index, int size, unsigned int type,
        int stride, const void *offset);

    void bind();
    void unbind();
    void destroy();
    unsigned int id;
};