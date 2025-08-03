#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "tmig/render/mesh.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

template<typename V>
Mesh<V>::Mesh() {
    glCreateVertexArrays(1, &vao); glCheckError();
    util::debugPrint("Created VAO: %u\n", vao);
}

template<typename V>
Mesh<V>::~Mesh() {
    util::debugPrint("Deleting VAO: %u\n", vao);
    glDeleteVertexArrays(1, &vao); glCheckError();
}

template<typename V>
Mesh<V>::Mesh(Mesh&& other) noexcept
    : vao{other.vao},
      vertexBuffer{other.vertexBuffer},
      indexBuffer{other.indexBuffer},
      vertexAttributes{std::move(other.vertexAttributes)}
{
    other.vao = 0;
    other.vertexBuffer = nullptr;
    other.indexBuffer = nullptr;
}

template<typename V>
Mesh<V>& Mesh<V>::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &vao); glCheckError();

        vao = other.vao;
        vertexBuffer = other.vertexBuffer;
        indexBuffer = other.indexBuffer;
        vertexAttributes = std::move(other.vertexAttributes);

        other.vao = 0;
        other.vertexBuffer = nullptr;
        other.indexBuffer = nullptr;
    }
    return *this;
}

template<typename V>
void Mesh<V>::setAttributes(const std::vector<VertexAttributeType>& _vertexAttributes) {
    if (_vertexAttributes.size() == 0) return;

    // Ensure stride size matches template type size
    size_t vertexStride = getStrideSize(_vertexAttributes.data(), _vertexAttributes.size());
    if (vertexStride != sizeof(V)) {
        throw std::runtime_error{"[Mesh::setAttributes] Vertex size doesn't match vertex stride"};
    }

    vertexAttributes = _vertexAttributes;
    configureVertexAttributes();
}

template<typename V>
void Mesh<V>::setVertexBuffer(DataBuffer<V>* buffer) {
    if (buffer == nullptr) return;

    if (vertexAttributes.empty()) {
#ifdef DEBUG
        throw std::runtime_error{"[Mesh::setAttributes] Need attribute layout to set buffer"};
#else
        return;
#endif
    }

    vertexBuffer = buffer;

    // Bind the buffer to binding index 0
    const uint32_t bindingIndex = 0;
    const size_t stride = getStrideSize(vertexAttributes.data(), vertexAttributes.size());
    glVertexArrayVertexBuffer(vao, bindingIndex, vertexBuffer->id(), 0, stride); glCheckError();
}

template<typename V>
void Mesh<V>::setIndexBuffer(DataBuffer<uint32_t>* buffer) {
    if (buffer == nullptr) return;

    indexBuffer = buffer;
    glVertexArrayElementBuffer(vao, indexBuffer->id()); glCheckError();
}

template<typename V>
void Mesh<V>::render() {
    if (indexBuffer == nullptr) return;

    glBindVertexArray(vao); glCheckError();
    glDrawElements(GL_TRIANGLES, indexBuffer->count(), GL_UNSIGNED_INT, 0); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V>
void Mesh<V>::configureVertexAttributes() {
    // Disable previously enabled attributes
    for (uint32_t i = 0; i < previousAttribCount; ++i) {
        glDisableVertexArrayAttrib(vao, i); glCheckError();
    }

    // Configure the vertex attributes to the VAO at binding index 0
    const uint32_t bindingIndex = 0;
    size_t vertexOffset = 0;
    uint32_t attribIndex = 0;

    // Set vertex attributes
    for (auto attr : vertexAttributes) {
        if (attr == VertexAttributeType::Mat4x4) {
            for (int i = 0; i < 4; ++i) {
                glVertexArrayAttribFormat(vao, attribIndex, 4, GL_FLOAT, GL_FALSE, vertexOffset + sizeof(glm::vec4) * i); glCheckError();
                glVertexArrayAttribBinding(vao, attribIndex, bindingIndex); glCheckError();
                glEnableVertexArrayAttrib(vao, attribIndex); glCheckError();
                ++attribIndex;
            }
        } else {
            glVertexArrayAttribFormat(vao, attribIndex, getAttributeCount(attr), getAttributeType(attr), GL_FALSE, vertexOffset); glCheckError();
            glVertexArrayAttribBinding(vao, attribIndex, bindingIndex); glCheckError();
            glEnableVertexArrayAttrib(vao, attribIndex); glCheckError();
            ++attribIndex;
        }

        vertexOffset += getAttributeSize(attr);
    }

    // Store how many attributes were used
    previousAttribCount = attribIndex;
}

} // namespace tmig::render