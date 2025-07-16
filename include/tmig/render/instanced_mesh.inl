#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "tmig/render/instanced_mesh.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

static size_t getAttributeSize(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Float:  return 1  * sizeof(float);
        case VertexAttributeType::Float2: return 2  * sizeof(float);
        case VertexAttributeType::Float3: return 3  * sizeof(float);
        case VertexAttributeType::Float4: return 4  * sizeof(float);
        case VertexAttributeType::Mat4x4: return 16 * sizeof(float);
    }
    return 0;
}

static size_t getAttributeCount(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Float:  return 1;
        case VertexAttributeType::Float2: return 2;
        case VertexAttributeType::Float3: return 3;
        case VertexAttributeType::Float4: return 4;
        case VertexAttributeType::Mat4x4: return 16;
    }
    return 0;
}

static size_t getAttributeType(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Float:  return GL_FLOAT;
        case VertexAttributeType::Float2: return GL_FLOAT;
        case VertexAttributeType::Float3: return GL_FLOAT;
        case VertexAttributeType::Float4: return GL_FLOAT;
        case VertexAttributeType::Mat4x4: return GL_FLOAT;
    }
    return 0;
}

template<typename V, typename I>
void InstancedMesh<V, I>::setAttributes(
    const std::vector<VertexAttributeType> &_vertexAttributes,
    const std::vector<VertexAttributeType> &_instanceAttributes
) {
    // Generate VAO if needed
    if (vao == 0) {
        glGenVertexArrays(1, &vao); glCheckError();
    }

    // Update attribute vectors
    vertexAttributes = _vertexAttributes;
    instanceAttributes = _instanceAttributes;

    // Delete old buffers
    if (vertexVbo != 0) {
        glDeleteBuffers(1, &vertexVbo); glCheckError();
    }
    if (instanceVbo != 0) {
        glDeleteBuffers(1, &instanceVbo); glCheckError();
    }

    // New vertex buffer
    glGenBuffers(1, &vertexVbo); glCheckError();

    // Check if need new instance buffer
    if (instanceAttributes.size() > 0) {
        glGenBuffers(1, &instanceVbo); glCheckError();
    } else {
        instanceVbo = 0;
    }

    configureVertexAttributes();
}

template<typename V, typename I>
void InstancedMesh<V, I>::setVertexBufferData(const V *data, size_t count) {
    if (vertexVbo == 0) return;

    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo); glCheckError();
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(V), data, GL_STATIC_DRAW); glCheckError();
}

template<typename V, typename I>
void InstancedMesh<V, I>::setInstanceBufferData(const I *data, size_t count) {
    if (instanceVbo == 0) return;

    instanceCount = static_cast<int>(count);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo); glCheckError();
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(I), data, GL_STATIC_DRAW); glCheckError();
}

template<typename V, typename I>
void InstancedMesh<V, I>::setIndexBufferData(const std::vector<unsigned int> &indices) {
    if (vertexVbo == 0) return;

    indexCount = static_cast<int>(indices.size());
    if (ebo == 0) {
        glGenBuffers(1, &ebo); glCheckError();
    }

    glBindVertexArray(vao); glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); glCheckError();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V, typename I>
void InstancedMesh<V, I>::render() {
    glBindVertexArray(vao); glCheckError();
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V, typename I>
void InstancedMesh<V, I>::configureVertexAttributes() {
    glBindVertexArray(vao); glCheckError();

    size_t vertexStride = 0;
    for (auto attr : vertexAttributes) {
        vertexStride += getAttributeSize(attr);
    }

    size_t instanceStride = 0;
    for (auto attr : instanceAttributes) {
        instanceStride += getAttributeSize(attr);
    }

    // Ensure stride size matches template type size
    util::debugPrint("vert str %ld | inst str %ld | sizeV %ld | sizeI %ld\n", vertexStride, instanceStride, sizeof(V), sizeof(I));
    if (vertexStride != sizeof(V)) {
        throw std::runtime_error{"Vertex type size does not match vertex stride size"};
    }
    if (instanceStride != sizeof(I)) {
        throw std::runtime_error{"Instance type size does not match instance stride size"};
    }
    
    // Set vertex attributes
    size_t vertexOffset = 0;
    GLuint attribIndex = 0;
    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo); glCheckError();
    for (auto attr : vertexAttributes) {
        if (attr == VertexAttributeType::Mat4x4) {
            for (int i = 0; i < 4; ++i) {
                glVertexAttribPointer(attribIndex, 4, GL_FLOAT, GL_FALSE, vertexStride, (void*)(vertexOffset + sizeof(glm::vec4) * i)); glCheckError();
                glEnableVertexAttribArray(attribIndex); glCheckError();
                attribIndex++;
            }
            vertexOffset += getAttributeSize(attr);
        } else {
            glVertexAttribPointer(attribIndex, getAttributeCount(attr), getAttributeType(attr), GL_FALSE, vertexStride, (void*)vertexOffset); glCheckError();
            glEnableVertexAttribArray(attribIndex); glCheckError();
            vertexOffset += getAttributeSize(attr);
            attribIndex++;
        }
    }

    // Set instance attributes
    size_t instanceOffset = 0;
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo); glCheckError();
    for (auto attr : instanceAttributes) {
        if (attr == VertexAttributeType::Mat4x4) {
            for (int i = 0; i < 4; ++i) {
                glEnableVertexAttribArray(attribIndex); glCheckError();
                glVertexAttribPointer(attribIndex, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(instanceOffset + sizeof(glm::vec4) * i)); glCheckError();
                glVertexAttribDivisor(attribIndex, 1); glCheckError();
                attribIndex++;
            }
        } else {
            glEnableVertexAttribArray(attribIndex); glCheckError();
            glVertexAttribPointer(attribIndex, getAttributeCount(attr), getAttributeType(attr), GL_FALSE, instanceStride, (void*)instanceOffset); glCheckError();
            glVertexAttribDivisor(attribIndex, 1); glCheckError();
            attribIndex++;
        }

        instanceOffset += getAttributeSize(attr);
    }

    glBindVertexArray(0); glCheckError();
}

} // namespace tmig::render