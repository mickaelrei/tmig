#pragma once

#include <vector>
#include <memory>

#include "tmig/util/debug.hpp"

namespace tmig::render {

/// @brief Enum for supported vertex attribute types. Used in Meshes to send vertex data
enum class VertexAttributeType {
    /// @brief Single float attribute
    Float,

    /// @brief Two floats attribute, used as vec2 in shader
    Float2,

    /// @brief Three floats attribute, used as vec3 in shader
    Float3,

    /// @brief Four floats attribute, used as vec4 in shader
    Float4,

    /// @brief 4x4 matrix attribute, used as mat4 in shader
    Mat4x4,
};

/// @brief Class for creating and rendering a mesh with instancing
/// @tparam V type used as vertex data
/// @tparam I type used as instance data
/// @note Make sure to call `setAttributes` before calling `setVertexBufferData`, `setInstanceBufferData`
/// and `setIndexBufferData`. Also make sure to pass matching attributes based on the two template typenames.
template<typename V, typename I>
class InstancedMesh {
public:
    /// @brief Default constructor
    InstancedMesh() = default;

    /// @brief Set attributes for this mesh
    /// @param vertexAttributes Attributes per vertex
    /// @param instanceAttributes Attributes per instance
    /// @note The attributes will be set in the order they're given: first vertex, then instance
    void setAttributes(
        const std::vector<VertexAttributeType> &vertexAttributes,
        const std::vector<VertexAttributeType> &instanceAttributes
    );

    /// @brief Set per-vertex data
    void setVertexBufferData(const V *data, size_t count);

    /// @brief Set per-instance data
    void setInstanceBufferData(const I *data, size_t count);

    /// @brief Set indices buffer data
    void setIndexBufferData(const std::vector<unsigned int> &indices);

    /// @brief Render this mesh
    void render();

private:
    /// @brief Vertex Attribute Object tied to this mesh
    unsigned int vao = 0;

    /// @brief Buffer data for per-vertex attributes
    unsigned int vertexVbo = 0;

    /// @brief Buffer data for per-instance attributes
    unsigned int instanceVbo = 0;

    /// @brief Elements/indices buffer data
    unsigned int ebo = 0;

    /// @brief How many indices are in the indices buffer
    int indexCount = 0;

    /// @brief How many instances are to be rendered
    int instanceCount = 0;

    /// @brief Per-vertex attributes
    std::vector<VertexAttributeType> vertexAttributes;

    /// @brief Per-instance attributes
    std::vector<VertexAttributeType> instanceAttributes;

    /// @brief Internally configure attributes
    void configureVertexAttributes();
};

} // namespace tmig::render

#include "tmig/render/instanced_mesh.inl"