#include "glad/glad.h"

#include "tmig/render/data_buffer.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

template<typename T>
DataBuffer<T>::DataBuffer() {
    glGenBuffers(1, &id); glCheckError();
    util::debugPrint("Created VBO %ld\n", id);
}

template<typename T>
DataBuffer<T>::~DataBuffer() {
    auto _id = id;
    glDeleteBuffers(1, &id); glCheckError();
    util::debugPrint("Deleted VBO %ld\n", _id);
}

template<typename T>
void DataBuffer<T>::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id); glCheckError();
}

template<typename T>
void DataBuffer<T>::setData(const T* data, size_t _count) {
    count = _count;
    bind();
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), data, GL_STATIC_DRAW); glCheckError();
}

template<typename T>
void DataBuffer<T>::setData(const std::vector<T> &vector) {
    setData(vector.data(), vector.size());
}

} // namespace tmig::render