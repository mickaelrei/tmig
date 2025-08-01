#pragma once

#include <cstdint>

#include "tmig/core/non_copyable.hpp"

namespace tmig::render {

/// @brief Class representing a GPU uniform buffer, used to store data used across multiple shaders
/// @tparam T type of data stored
/// @note - You MUST call `setBindingPoint` at some point, it won't be called automatically
/// @note - This buffer is supposed to match with a std140 layout memory block in a shader. Make sure you
///         understand std140 layout rules to correctly send data
/// @note - This is a non-copyable class, meaning you cannot create a copy of it
template<typename T>
class UniformBuffer : protected core::NonCopyable {
public:
    /// @brief Constructor
    UniformBuffer();

    /// @brief Constructor
    virtual ~UniformBuffer();

    /// @brief Move constructor
    UniformBuffer(UniformBuffer&& other) noexcept;

    /// @brief Move assignment operator
    UniformBuffer& operator=(UniformBuffer&& other) noexcept;

    /// @brief Set buffer data
    void setData(const T& data);

    /// @brief Set buffer binding point
    void bindTo(uint32_t index);

private:
    uint32_t _id = 0;
    uint32_t bindingPoint = 0;
};

} // namespace tmig::render

#include "tmig/render/uniform_buffer.inl"