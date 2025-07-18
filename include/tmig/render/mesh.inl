#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "tmig/render/mesh.hpp"
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

template<typename V>
Mesh<V>::Mesh() {
    glGenVertexArrays(1, &vao); glCheckError();
    util::debugPrint("Created VAO %ld\n", vao);
}

template<typename V>
Mesh<V>::~Mesh() {
    auto _vao = vao;
    glDeleteVertexArrays(1, &vao); glCheckError();
    util::debugPrint("Deleted VAO %ld\n", _vao);
}

template<typename V>
void Mesh<V>::setAttributes(const std::vector<VertexAttributeType> &_vertexAttributes) {
    vertexAttributes = _vertexAttributes;
    vertexAttributesConfigured = false;
}

template<typename V>
void Mesh<V>::setVertexBuffer(std::shared_ptr<DataBuffer<V>> buffer) {
    if (vertexAttributes.empty()) {
        throw std::runtime_error{"Cannot bind vertex buffer without attribute layout (setAttributes)"};
    }
    
    vertexBuffer = buffer;
    configureVertexAttributes();
    vertexAttributesConfigured = true;
}

template<typename V>
void Mesh<V>::setIndexBufferData(const std::vector<unsigned int> &indices) {
    indexCount = static_cast<int>(indices.size());
    if (ebo == 0) {
        glGenBuffers(1, &ebo); glCheckError();
    }

    glBindVertexArray(vao); glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); glCheckError();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V>
void Mesh<V>::render() {
    glBindVertexArray(vao); glCheckError();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); glCheckError();
    glBindVertexArray(0); glCheckError();
}

template<typename V>
void Mesh<V>::configureVertexAttributes() {
    glBindVertexArray(vao); glCheckError();

    size_t vertexStride = 0;
    for (auto attr : vertexAttributes) {
        vertexStride += getAttributeSize(attr);
    }

    // Ensure stride size matches template type size
    util::debugPrint("vertex stride: %ld | sizeof(V): %ld\n", vertexStride, sizeof(V));
    if (vertexStride != sizeof(V)) {
        throw std::runtime_error{"Vertex type size does not match vertex stride size"};
    }
    
    // Set vertex attributes
    size_t vertexOffset = 0;
    GLuint attribIndex = 0;
    vertexBuffer->bind();
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

    glBindVertexArray(0); glCheckError();
}

} // namespace tmig::render