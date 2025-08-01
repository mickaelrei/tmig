#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "tmig/core/non_copyable.hpp"
#include "tmig/render/vertex_attribute.hpp"
#include "tmig/render/data_buffer.hpp"

namespace tmig::render {

/// @brief Class for creating and rendering a mesh
/// @tparam V type used as vertex data
/// @note - Make sure to call `setAttributes` before calling `setVertexBuffer`.
///         Also make sure to pass matching attributes based on the template typename.
/// @note - This is a non-copyable class, meaning you cannot create a copy of it.
///         See `InstancedMesh` for rendering "copies" of the same mesh
template<typename V>
class Mesh : protected core::NonCopyable {
public:
    /// @brief Constructor
    Mesh();

    /// @brief Destructor
    virtual ~Mesh();

    /// @brief Move constructor
    Mesh(Mesh&& other) noexcept;

    /// @brief Move assignment operator
    Mesh& operator=(Mesh&& other) noexcept;

    /// @brief Set attributes for this mesh
    /// @param vertexAttributes Attributes per vertex
    void setAttributes(const std::vector<VertexAttributeType>& vertexAttributes);

    /// @brief Set per-vertex buffer
    virtual void setVertexBuffer(DataBuffer<V>* buffer);

    /// @brief Set indices buffer
    virtual void setIndexBuffer(DataBuffer<uint32_t>* buffer);

    /// @brief Render this mesh
    virtual void render();

protected:
    /// @brief Vertex Attribute Object tied to this mesh
    uint32_t vao = 0;

    /// @brief Pointer to vertex buffer
    DataBuffer<V>* vertexBuffer;

    /// @brief Pointer to index buffer
    DataBuffer<uint32_t>* indexBuffer;

    /// @brief Per-vertex attributes
    std::vector<VertexAttributeType> vertexAttributes;

    /// @brief Internally configure per-vertex attributes
    void configureVertexAttributes();
};

} // namespace tmig::render

#include "tmig/render/mesh.inl"