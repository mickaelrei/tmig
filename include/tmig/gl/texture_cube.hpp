#pragma once

#include <string>

namespace tmig {

namespace gl {

/// @brief OpenGL texture cube wrapper class
class TextureCube {
public:
    /// @brief Default constructor (does nothing)
    TextureCube() = default;

    /// @brief Constructor with image file paths
    /// @param rightPath path to right side of texture
    /// @param leftPath path to left side of texture
    /// @param topPath path to top side of texture
    /// @param bottomPath path to bottom side of texture
    /// @param frontPath path to front side of texture
    /// @param backPath path to back side of texture
    TextureCube(
        const std::string &rightPath,
        const std::string &leftPath,
        const std::string &topPath,
        const std::string &bottomPath,
        const std::string &frontPath,
        const std::string &backPath
    );

    /// @brief Bind texture
    void bind() const;

    /// @brief Unbind texture
    void unbind() const;

    /// @brief Destroy texture resources
    void destroy() const;

private:
    /// @brief OpenGL object identifier
    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig