#pragma once

#include <vector>

class EBO
{
public:
    EBO();
    EBO(const std::vector<unsigned int> &indices);

    void bufferData(size_t size, const void *data);

    void bind();
    void unbind();
    void destroy();

    unsigned int id;
};