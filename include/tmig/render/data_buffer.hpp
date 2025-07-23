#pragma once

#include <vector>
#include <cstdint>

namespace tmig::render {

/// @brief Class representing a GPU buffer data store
/// @tparam T type of data stored
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
    /// @note Buffer is entirely reallocated to handle different count; if same count as previous, consider using `setSubset`
    void setData(const T *data, size_t count);

    /// @brief Set buffer data
    /// @note Buffer is entirely reallocated to handle different count; if same count as previous, consider using `setSubset`
    void setData(const std::vector<T> &vector);

    /// @brief Set subset of data
    /// @param offset Data start offset
    /// @param count How much data is to be set
    /// @param data Pointer to start of data
    /// @note - Make sure that the `data` pointer based on `count`
    /// @note - Make sure that the `[offset, offset + count]` range is within bounds. Check with `count`
    void setSubset(size_t offset, size_t count, const T *data);

    /// @brief Get current buffer data count
    size_t count() const { return _count; }

    /// @brief Get OpenGL identifier; used internally
    uint32_t id() const { return _id; }

private:
    /// @brief OpenGL identifier
    uint32_t _id = 0;

    /// @brief Buffer current count
    size_t _count = 0;
};

} // namespace tmig::render

#include "tmig/render/data_buffer.inl"