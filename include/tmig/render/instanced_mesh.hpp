#pragma once

#include <vector>
#include <memory>

#include "tmig/render/mesh.hpp"

namespace tmig::render {

/// @brief Class for creating and rendering a mesh with instancing
/// @tparam V type used as vertex data
/// @tparam I type used as instance data
/// @note - Make sure to call `setAttributes` before calling `setVertexBuffer`. Also make sure to pass
/// matching attributes based on the two template typenames.
/// @note - This is a non-copyable class, meaning you cannot create a copy of it.
template<typename V, typename I>
class InstancedMesh : private Mesh<V> {
public:
    /// @brief Default constructor
    InstancedMesh() = default;

    /// @brief Move constructor
    InstancedMesh(InstancedMesh&& other) noexcept;

    /// @brief Move assignment operator
    InstancedMesh& operator=(InstancedMesh&& other) noexcept;

    /// @brief Set attributes for this mesh
    /// @param vertexAttributes Attributes per vertex
    /// @param instanceAttributes Attributes per instance
    /// @note The attributes will be set in the order they're given: first vertex, then instance
    void setAttributes(
        const std::vector<VertexAttributeType>& vertexAttributes,
        const std::vector<VertexAttributeType>& instanceAttributes
    );

    /// @brief Set per-instance buffer
    void setInstanceBuffer(DataBuffer<I>* buffer);

    /// @brief Set per-vertex buffer
    void setVertexBuffer(DataBuffer<V>* buffer) override {
        Mesh<V>::setVertexBuffer(buffer);
    }

    /// @brief Set indices buffer data
    void setIndexBuffer(DataBuffer<uint32_t>* buffer) override {
        Mesh<V>::setIndexBuffer(buffer);
    }

    /// @brief Render this mesh
    void render() override;

protected:
    /// @brief Pointer to instance buffer
    DataBuffer<I>* instanceBuffer;

    /// @brief Per-instance attributes
    std::vector<VertexAttributeType> instanceAttributes;

    /// @brief Whether instance attributes are configured
    bool instanceAttributesConfigured = false;

    /// @brief Internally configure per-instance attributes
    void configureInstanceAttributes();
};

} // namespace tmig::render

#include "tmig/render/instanced_mesh.inl"