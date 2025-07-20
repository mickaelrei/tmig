#include "glad/glad.h"

#include "tmig/render/data_buffer.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

template<typename T>
DataBuffer<T>::DataBuffer() {
    glCreateBuffers(1, &_id); glCheckError();
    util::debugPrint("Created VBO %ld\n", _id);
}

template<typename T>
DataBuffer<T>::~DataBuffer() {
    util::debugPrint("Deleting VBO %ld\n", _id);
    glDeleteBuffers(1, &_id); glCheckError();
}

template<typename T>
void DataBuffer<T>::setData(const T* data, size_t count) {
    _count = count;
    glNamedBufferData(_id, _count * sizeof(T), data, GL_STATIC_DRAW); glCheckError();
}

template<typename T>
void DataBuffer<T>::setData(const std::vector<T> &vector) {
    setData(vector.data(), vector.size());
}

} // namespace tmig::render