#pragma once

#include <memory>

#include "tmig/gl/gl_object.hpp"

namespace tmig {

namespace gl {

/// @brief OpenGL texture wrapper class
class Texture : public GLObject {
public:
    /// @brief Default constructor (does nothing)
    Texture() = default;

    /// @brief Creates a new Texture
    /// @param path path to image file
    /// @return Shared pointer to new texture
    static std::shared_ptr<Texture> create(const std::string &path);

    /// @brief Activate texture
    /// @param unit texture unit to be set to
    void activate(const unsigned int unit) const;

    /// @brief Bind texture
    void bind() const;

    /// @brief Unbind texture
    void unbind() const;

    /// @brief Destroy texture
    void destroy() override;

private:
    /// @brief Constructor with path to file
    /// @param path path to image file
    Texture(const std::string &path);
};

} // namespace gl

} // namespace tmig