#pragma once

#include <vector>

#include "tmig/structs.hpp"

namespace tmig {

class Mesh
{
public:
    Mesh();
    Mesh(
        const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices
    );

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};

} // namespace tmig