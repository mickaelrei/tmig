#pragma once

#include <vector>

#include "structs.hpp"

class Mesh
{
public:
    Mesh();
    Mesh(
        const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};