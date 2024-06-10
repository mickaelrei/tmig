#pragma once

#include <vector>
#include <cstddef>

class EBO
{
public:
    EBO();
    EBO(const std::vector<unsigned int> &indices);

    void bufferData(size_t size, const void *data) const;

    void bind() const;
    void unbind() const;
    void destroy() const;

    unsigned int id = 0;
};