#pragma once

namespace tmig {

namespace gl {

/// @brief OpenGL texture wrapper class
class Texture {
public:
    /// @brief Default constructor (does nothing)
    Texture() = default;

    /// @brief Constructor with path to file
    /// @param path path to image file
    Texture(const std::string &path);

    /// @brief Activate texture
    /// @param unit texture unit to be set to
    void activate(const unsigned int unit) const;

    /// @brief Bind texture
    void bind() const;

    /// @brief Unbind texture
    void unbind() const;

    /// @brief Destroy texture
    void destroy() const;

private:
    /// @brief OpenGL object identifier
    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig