#pragma once

#include <vector>

namespace tmig::render {

/// @brief Class representing a GPU Vertex Buffer
/// @tparam T 
template<typename T>
class DataBuffer {
public:
    /// @brief Constructor
    /// @note Creates the buffer
    DataBuffer();

    /// @brief Constructor
    /// @note Deletes the buffer
    ~DataBuffer();

    /// @brief Set buffer data
    void setData(const T *data, size_t count);

    /// @brief Set buffer data
    void setData(const std::vector<T> &vector);

    /// @brief Get current buffer data count
    size_t count() const { return _count; }

    unsigned int id() const { return _id; }

private:
    /// @brief OpenGL identifier
    unsigned int _id = 0;

    /// @brief Buffer current count
    size_t _count = 0;

    /// @brief Buffer max capacity
    size_t capacity = 0;

    /// @brief Allocate storage for the buffer
    void allocate(size_t newCapacity);
};

} // namespace tmig::render

#include "tmig/render/data_buffer.inl"