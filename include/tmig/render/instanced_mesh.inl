#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "tmig/render/instanced_mesh.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

template<typename V, typename I>
void InstancedMesh<V, I>::setAttributes(
    const std::vector<VertexAttributeType> &_vertexAttributes,
    const std::vector<VertexAttributeType> &_instanceAttributes
) {
    Mesh<V>::setAttributes(_vertexAttributes);
    instanceAttributes = _instanceAttributes;
    instanceAttributesConfigured = false;
}

template<typename V, typename I>
void InstancedMesh<V, I>::setInstanceBuffer(std::shared_ptr<DataBuffer<I>> buffer) {
    if (Mesh<V>::vertexAttributes.empty() || Mesh<V>::vertexBuffer == nullptr) {
        throw std::runtime_error{"Cannot bind instance buffer without vertex buffer"};
    }

    if (instanceAttributes.empty()) {
        throw std::runtime_error{"Cannot bind instance buffer without attribute layout (setAttributes)"};
    }
    
    instanceBuffer = buffer;
    configureVertexAttributes();
}

template<typename V, typename I>
void InstancedMesh<V, I>::render() {
    if (instanceBuffer == nullptr) return;

    util::debugPrint("glDrawElementsInstanced(GL_TRIANGLES, %ld, GL_UNSIGNED_INT, 0, %ld);\n", Mesh<V>::indexCount, instanceBuffer->getCount());
    glBindVertexArray(Mesh<V>::vao); glCheckError();
    glDrawElementsInstanced(GL_TRIANGLES, Mesh<V>::indexCount, GL_UNSIGNED_INT, 0, instanceBuffer->getCount()); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V, typename I>
unsigned int InstancedMesh<V, I>::configureVertexAttributes() {
    auto attribIndex = Mesh<V>::configureVertexAttributes();

    // This could be called right after setVertexBuffer when setInstanceBuffer wasn't called
    if (instanceAttributes.size() == 0 || instanceBuffer == nullptr) {
        return attribIndex;
    }

    glBindVertexArray(Mesh<V>::vao); glCheckError();

    // Ensure stride matches template type size
    size_t instanceStride = 0;
    for (auto attr : instanceAttributes) {
        instanceStride += getAttributeSize(attr);
    }

    if (instanceStride != sizeof(I)) {
        throw std::runtime_error{"Instance type size does not match instance stride size"};
    }

    // Set instance attributes
    size_t instanceOffset = 0;
    instanceBuffer->bind();
    for (auto attr : instanceAttributes) {
        if (attr == VertexAttributeType::Mat4x4) {
            for (int i = 0; i < 4; ++i) {
                util::debugPrint("glVertexAttribPointer(%ld, 4, %ld, GL_FALSE, %ld, (void*)%ld);\n", attribIndex, GL_FLOAT, instanceStride, instanceOffset + sizeof(glm::vec4) * i);
                glEnableVertexAttribArray(attribIndex); glCheckError();
                glVertexAttribPointer(attribIndex, 4, GL_FLOAT, GL_FALSE, instanceStride, (void*)(instanceOffset + sizeof(glm::vec4) * i)); glCheckError();
                glVertexAttribDivisor(attribIndex, 1); glCheckError();
                attribIndex++;
            }
        } else {
            util::debugPrint("glVertexAttribPointer(%ld, %ld, %ld, GL_FALSE, %ld, (void*)%ld);\n", attribIndex, getAttributeCount(attr), getAttributeType(attr), instanceStride, instanceOffset);
            glEnableVertexAttribArray(attribIndex); glCheckError();
            glVertexAttribPointer(attribIndex, getAttributeCount(attr), getAttributeType(attr), GL_FALSE, instanceStride, (void*)instanceOffset); glCheckError();
            glVertexAttribDivisor(attribIndex, 1); glCheckError();
            attribIndex++;
        }

        instanceOffset += getAttributeSize(attr);
    }

    glBindVertexArray(0); glCheckError();
    return attribIndex;
}

} // namespace tmig::render