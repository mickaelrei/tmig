#include "glad/glad.h"

#include "tmig/render/vertex_attribute.hpp"

namespace tmig::render {

size_t getAttributeSize(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::FLOAT:  return 1  * sizeof(float);
        case VertexAttributeType::FLOAT2: return 2  * sizeof(float);
        case VertexAttributeType::FLOAT3: return 3  * sizeof(float);
        case VertexAttributeType::FLOAT4: return 4  * sizeof(float);
        case VertexAttributeType::MAT4x4: return 16 * sizeof(float);
    }
    return 0;
}

size_t getAttributeCount(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::FLOAT:  return 1;
        case VertexAttributeType::FLOAT2: return 2;
        case VertexAttributeType::FLOAT3: return 3;
        case VertexAttributeType::FLOAT4: return 4;
        case VertexAttributeType::MAT4x4: return 16;
    }
    return 0;
}

size_t getAttributeType(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::FLOAT:  return GL_FLOAT;
        case VertexAttributeType::FLOAT2: return GL_FLOAT;
        case VertexAttributeType::FLOAT3: return GL_FLOAT;
        case VertexAttributeType::FLOAT4: return GL_FLOAT;
        case VertexAttributeType::MAT4x4: return GL_FLOAT;
    }
    return 0;
}

size_t getStrideSize(const VertexAttributeType *types, size_t count) {
    size_t stride = 0;
    for (size_t i = 0; i < count; ++i) {
        stride += getAttributeSize(types[i]);
    }
    return stride;
}

} // namespace tmig::render