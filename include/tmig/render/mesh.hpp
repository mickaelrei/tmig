#pragma once

#include <vector>
#include <memory>

#include "tmig/render/vertex_attribute.hpp"
#include "tmig/render/data_buffer.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

/// @brief Class for creating and rendering a mesh
/// @tparam V type used as vertex data
/// @note Make sure to call `setAttributes` before calling `setVertexBuffer`.
/// Also make sure to pass matching attributes based on the template typename.
template<typename V>
class Mesh {
public:
    /// @brief Constructor
    Mesh();

    /// @brief Destructor
    ~Mesh();

    /// @brief Set attributes for this mesh
    /// @param vertexAttributes Attributes per vertex
    void setAttributes(const std::vector<VertexAttributeType> &vertexAttributes);

    /// @brief Set per-vertex buffer
    virtual void setVertexBuffer(std::shared_ptr<DataBuffer<V>> buffer);

    /// @brief Set indices buffer
    virtual void setIndexBuffer(std::shared_ptr<DataBuffer<unsigned int>> buffer);

    /// @brief Render this mesh
    virtual void render();

protected:
    /// @brief Vertex Attribute Object tied to this mesh
    unsigned int vao = 0;

    /// @brief Pointer to vertex buffer
    std::shared_ptr<DataBuffer<V>> vertexBuffer;

    /// @brief Pointer to index buffer
    std::shared_ptr<DataBuffer<unsigned int>> indexBuffer;

    /// @brief Per-vertex attributes
    std::vector<VertexAttributeType> vertexAttributes;

    /// @brief Internally configure per-vertex attributes
    void configureVertexAttributes();
};

} // namespace tmig::render

#include "tmig/render/mesh.inl"