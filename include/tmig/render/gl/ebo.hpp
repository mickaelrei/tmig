#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "tmig/render/gl/gl_object.hpp"

namespace tmig {

namespace render {

namespace gl {

/// @brief OpenGL Element Buffer Object (EBO) wrapper class
class EBO : public GLObject {
public:
    /// @brief Default constructor (does nothing)
    EBO() = default;

    /// @brief Creates an instance of EBO
    /// @param indices list of indices/elements
    /// @return Shared pointer to new EBO
    static std::shared_ptr<EBO> create(const std::vector<unsigned int> &indices);

    /// @brief Set buffer data
    /// @param size buffer size
    /// @param data pointer to data
    void bufferData(size_t size, const void *data) const;

    /// @brief Bind object
    void bind() const;

    /// @brief Unbind object
    void unbind() const;

    /// @brief Destroy object
    void destroy() override;

private:
    /// @brief Constructor
    /// @param indices list of indices/elements
    EBO(const std::vector<unsigned int> &indices);
};

} // namespace gl

} // namespace render

} // namespace tmig