#include "glad/glad.h"

#include "tmig/render/uniform_buffer.hpp"

namespace tmig::render {

template<typename T>
UniformBuffer<T>::UniformBuffer() {
    glCreateBuffers(1, &_id); glCheckError();
    glNamedBufferStorage(_id, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT); glCheckError();
    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "Created UBO: %u\n", _id
    );
}

template<typename T>
UniformBuffer<T>::~UniformBuffer() {
    if (_id == 0) return;

    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "Deleting UBO: %u\n", _id
    );
    glDeleteBuffers(1, &_id); glCheckError();
}

template<typename T>
UniformBuffer<T>::UniformBuffer(UniformBuffer&& other) noexcept
    : _id{other._id}
{
    other._id = 0;
}

template<typename T>
UniformBuffer<T>& UniformBuffer<T>::operator=(UniformBuffer&& other) noexcept {
    if (this != &other) {
        if (_id != 0) {
            glDeleteBuffers(1, &_id); glCheckError();
        }

        _id = other._id;
        other._id = 0;
    }
    return *this;
}

template<typename T>
void UniformBuffer<T>::setData(const T& data) {
    glNamedBufferSubData(_id, 0, sizeof(T), &data); glCheckError();
}

template<typename T>
void UniformBuffer<T>::bindTo(uint32_t binding) {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, _id); glCheckError();
}

} // namespace tmig::render