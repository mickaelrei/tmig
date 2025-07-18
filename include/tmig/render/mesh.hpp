#pragma once

#include <vector>
#include <memory>

#include "tmig/render/data_buffer.hpp"
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

/// @brief Class for creating and rendering a mesh
/// @tparam V type used as vertex data
/// @note Make sure to call `setAttributes` before calling `setVertexBufferData` and `setIndexBufferData`.
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
    void setVertexBuffer(std::shared_ptr<DataBuffer<V>> buffer);

    /// @brief Set indices buffer data
    void setIndexBufferData(const std::vector<unsigned int> &indices);

    /// @brief Render this mesh
    void render();

private:
    /// @brief Vertex Attribute Object tied to this mesh
    unsigned int vao = 0;

    /// @brief Pointer to vertex buffer
    std::shared_ptr<DataBuffer<V>> vertexBuffer;

    /// @brief Elements/indices buffer data
    unsigned int ebo = 0;

    /// @brief How many indices are in the indices buffer
    int indexCount = 0;

    /// @brief Per-vertex attributes
    std::vector<VertexAttributeType> vertexAttributes;

    /// @brief Whether vertex attributes are configured
    bool vertexAttributesConfigured = false;

    /// @brief Internally configure attributes
    void configureVertexAttributes();
};

} // namespace tmig::render

#include "tmig/render/mesh.inl"