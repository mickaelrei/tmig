#include "glad/glad.h"

#include "tmig/render/vertex_attribute.hpp"

namespace tmig::render {

size_t getAttributeSize(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Float:  return 1  * sizeof(float);
        case VertexAttributeType::Float2: return 2  * sizeof(float);
        case VertexAttributeType::Float3: return 3  * sizeof(float);
        case VertexAttributeType::Float4: return 4  * sizeof(float);
        case VertexAttributeType::Mat4x4: return 16 * sizeof(float);
    }
    return 0;
}

size_t getAttributeCount(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Float:  return 1;
        case VertexAttributeType::Float2: return 2;
        case VertexAttributeType::Float3: return 3;
        case VertexAttributeType::Float4: return 4;
        case VertexAttributeType::Mat4x4: return 16;
    }
    return 0;
}

size_t getAttributeType(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Float:  return GL_FLOAT;
        case VertexAttributeType::Float2: return GL_FLOAT;
        case VertexAttributeType::Float3: return GL_FLOAT;
        case VertexAttributeType::Float4: return GL_FLOAT;
        case VertexAttributeType::Mat4x4: return GL_FLOAT;
    }
    return 0;
}

} // namespace tmig::render