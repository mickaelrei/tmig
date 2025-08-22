#pragma once

#include <vector>
#include <cstdint>

#include "tmig/core/non_copyable.hpp"

namespace tmig::render {

/// @brief Class representing a GPU buffer data store
/// @tparam T type of data stored
/// @note - This is a non-copyable class, meaning you cannot create a copy of it
template<typename T>
class DataBuffer : protected core::NonCopyable {
public:
    /// @brief Constructor
    DataBuffer();

    /// @brief Constructor
    virtual ~DataBuffer();

    /// @brief Move constructor
    DataBuffer(DataBuffer&& other) noexcept;

    /// @brief Move assignment operator
    DataBuffer& operator=(DataBuffer&& other) noexcept;

    /// @brief Set buffer data
    /// @note Buffer is entirely reallocated to handle different count; if same count as previous, consider using `setSubset`
    void setData(const T* data, size_t count);

    /// @brief Set buffer data
    /// @note Buffer is entirely reallocated to handle different count; if same count as previous, consider using `setSubset`
    void setData(const std::vector<T>& vector);

    /// @brief Set subset of data
    /// @param offset Data start offset
    /// @param count How much data is to be set
    /// @param data Pointer to start of data
    /// @note - Make sure that the `data` pointer based on `count`
    /// @note - Make sure that the `[offset, offset + count]` range is within bounds. Check with `count`
    void setSubset(size_t offset, size_t count, const T* data);

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