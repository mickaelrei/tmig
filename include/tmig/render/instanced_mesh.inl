#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "tmig/render/instanced_mesh.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

template<typename V, typename I>
InstancedMesh<V, I>::InstancedMesh(InstancedMesh&& other) noexcept
    : Mesh<V>{std::move(other)},
     instanceBuffer{other.instanceBuffer},
     instanceAttributes{std::move(other.instanceAttributes)},
     instanceAttributesConfigured{other.instanceAttributesConfigured}
{
    other.instanceBuffer = nullptr;
    other.instanceAttributesConfigured = false;
}

template<typename V, typename I>
InstancedMesh<V, I>& InstancedMesh<V, I>::operator=(InstancedMesh&& other) noexcept {
    if (this != &other) {
        Mesh<V>::operator=(std::move(other));
        instanceBuffer = other.instanceBuffer;
        instanceAttributes = std::move(other.instanceAttributes);
        instanceAttributesConfigured = other.instanceAttributesConfigured;

        other.instanceBuffer = nullptr;
        other.instanceAttributesConfigured = false;
    }
    return *this;
}

template<typename V, typename I>
void InstancedMesh<V, I>::setAttributes(
    const std::vector<VertexAttributeType>& _vertexAttributes,
    const std::vector<VertexAttributeType>& _instanceAttributes
) {
    if (_vertexAttributes.size() == 0 || _instanceAttributes.size() == 0) return;

    // Ensure stride size matches template types size
    size_t vertexStride = getStrideSize(_vertexAttributes.data(), _vertexAttributes.size());
    util::debugPrint("vertex stride: %ld | sizeof(V): %ld\n", vertexStride, sizeof(V));
    if (vertexStride != sizeof(V)) {
        throw std::runtime_error{"Vertex type size does not match vertex stride size"};
    }
    size_t instanceStride = getStrideSize(_instanceAttributes.data(), _instanceAttributes.size());
    util::debugPrint("instance stride: %ld | sizeof(I): %ld\n", instanceStride, sizeof(I));
    if (instanceStride != sizeof(I)) {
        throw std::runtime_error{"Instance type size does not match instance stride size"};
    }

    Mesh<V>::setAttributes(_vertexAttributes);
    instanceAttributes = _instanceAttributes;
    configureInstanceAttributes();
}

template<typename V, typename I>
void InstancedMesh<V, I>::setInstanceBuffer(DataBuffer<I>* buffer) {
    if (buffer == nullptr) return;

    if (instanceAttributes.empty()) {
        throw std::runtime_error{"Cannot bind instance buffer without attribute layout (setAttributes)"};
    }
    instanceBuffer = buffer;

    // Only bind the buffer, do not reconfigure attributes
    const GLuint instanceBindingIndex = 1;
    const size_t stride = getStrideSize(instanceAttributes.data(), instanceAttributes.size());
    glVertexArrayVertexBuffer(Mesh<V>::vao, instanceBindingIndex, instanceBuffer->id(), 0, stride); glCheckError();
}

template<typename V, typename I>
void InstancedMesh<V, I>::render() {
    if (Mesh<V>::indexBuffer == nullptr || instanceBuffer == nullptr) return;

    glBindVertexArray(Mesh<V>::vao); glCheckError();
    glDrawElementsInstanced(GL_TRIANGLES, Mesh<V>::indexBuffer->count(), GL_UNSIGNED_INT, 0, instanceBuffer->count()); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V, typename I>
void InstancedMesh<V, I>::configureInstanceAttributes() {
    // Find the starting attribute index (after per-vertex attributes)
    GLuint attribIndex = 0;
    for (auto attr : Mesh<V>::vertexAttributes) {
        if (attr == VertexAttributeType::Mat4x4) {
            attribIndex += 4;
        } else {
            attribIndex += 1;
        }
    }

    // Configure the instance attributes to the VAO at binding index 1 to not conflict with per-vertex attributes at binding 0
    const GLuint bindingIndex = 1;
    size_t instanceOffset = 0;

    // Set instance attributes
    for (auto attr : instanceAttributes) {
        if (attr == VertexAttributeType::Mat4x4) {
            for (int i = 0; i < 4; ++i) {
                glVertexArrayAttribFormat(Mesh<V>::vao, attribIndex, 4, GL_FLOAT, GL_FALSE, instanceOffset + sizeof(glm::vec4) * i); glCheckError();
                glVertexArrayAttribBinding(Mesh<V>::vao, attribIndex, bindingIndex); glCheckError();
                glEnableVertexArrayAttrib(Mesh<V>::vao, attribIndex); glCheckError();
                glVertexArrayBindingDivisor(Mesh<V>::vao, bindingIndex, 1); glCheckError();
                attribIndex++;
            }
        } else {
            glVertexArrayAttribFormat(Mesh<V>::vao, attribIndex, getAttributeCount(attr), getAttributeType(attr), GL_FALSE, instanceOffset); glCheckError();
            glVertexArrayAttribBinding(Mesh<V>::vao, attribIndex, bindingIndex); glCheckError();
            glEnableVertexArrayAttrib(Mesh<V>::vao, attribIndex); glCheckError();
            glVertexArrayBindingDivisor(Mesh<V>::vao, bindingIndex, 1); glCheckError();
            attribIndex++;
        }

        instanceOffset += getAttributeSize(attr);
    }
}

} // namespace tmig::render