#pragma once

#include <cstddef>
#include <vector>

namespace tmig {

namespace gl {

/// @brief OpenGL Element Buffer Object (EBO) wrapper class
class EBO {
public:
    /// @brief Default constructor (does nothing)
    EBO() = default;

    /// @brief Constructor
    /// @param indices list of indices/elements
    EBO(const std::vector<unsigned int> &indices);

    /// @brief Set buffer data
    /// @param size buffer size
    /// @param data pointer to data
    void bufferData(size_t size, const void *data) const;

    /// @brief Bind object
    void bind() const;

    /// @brief Unbind object
    void unbind() const;

    /// @brief Destroy object
    void destroy() const;

private:
    /// @brief OpenGL object identifier
    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig