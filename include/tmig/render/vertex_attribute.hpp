#pragma once

#include <cstddef>

namespace tmig::render {

/// @brief Enum for supported vertex attribute types. Used in Meshes to send vertex data
enum class VertexAttributeType {
    /// @brief Single float attribute
    FLOAT,

    /// @brief Two floats attribute, used as vec2 in shader
    FLOAT2,

    /// @brief Three floats attribute, used as vec3 in shader
    FLOAT3,

    /// @brief Four floats attribute, used as vec4 in shader
    FLOAT4,

    /// @brief 4x4 matrix attribute, used as mat4 in shader
    MAT4x4,
};

/// @brief Get byte count for a given attribute type
size_t getAttributeSize(VertexAttributeType type);

/// @brief Get how many components does the given attribute type represents
///
/// - `VertexAttributeType::Float3` represents 3 floats, thus it returns 3
///
/// - `VertexAttributeType::Mat4x4` represents 16 floats, thus it returns 16
size_t getAttributeCount(VertexAttributeType type);

/// @brief Returns the OpenGL type for this attribute type
size_t getAttributeType(VertexAttributeType type);

/// @brief Helper function for getting a stride size based on a list of attribute types
size_t getStrideSize(const VertexAttributeType *types, size_t count);

} // namespace tmig::render