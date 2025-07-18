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

    /// @brief Binds this buffer
    void bind();

    /// @brief Set buffer data
    void setData(const T *data, size_t count);

    /// @brief Set buffer data
    void setData(const std::vector<T> &vector);

    /// @brief Get current buffer data count
    size_t getCount() const { return count; }

private:
    /// @brief OpenGL identifier
    unsigned int id;

    /// @brief Data count
    size_t count;
};

} // namespace tmig::render

#include "tmig/render/data_buffer.inl"