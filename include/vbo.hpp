#pragma once

#include <vector>
#include <concepts>

#include "structs.hpp"

class VBO
{
public:
    VBO();
    VBO(const std::vector<Vertex> &vertices);

    void bufferData(size_t size, const void *data);

    template<std::derived_from<Vertex> T>
    void bufferDmadmadmamda();

    void bind();
    void unbind();
    void destroy();

    unsigned int id;
};

template<std::derived_from<Vertex> T>
void VBO::bufferDmadmadmamda()
{
    printf("tmig is %ld\n", sizeof(T));
}