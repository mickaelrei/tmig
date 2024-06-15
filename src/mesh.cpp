#include "mesh.hpp"

namespace tmig {

Mesh::Mesh()
{
}

Mesh::Mesh(
    const std::vector<Vertex> &vertices,
    const std::vector<unsigned int> &indices
)
    : vertices{vertices},
    indices{indices} {}

} // namespace tmig