#pragma once

#include <string>
#include <memory>

#include "tmig/gl/gl_object.hpp"

namespace tmig {

namespace gl {

/// @brief OpenGL texture cube wrapper class
class TextureCube : public GLObject {
public:
    /// @brief Default constructor (does nothing)
    TextureCube() = default;

    /// @brief Create new texture cube
    /// @param rightPath path to right side of texture
    /// @param leftPath path to left side of texture
    /// @param topPath path to top side of texture
    /// @param bottomPath path to bottom side of texture
    /// @param frontPath path to front side of texture
    /// @param backPath path to back side of texture
    /// @return Shared pointer to new texture cube
    static std::shared_ptr<TextureCube> create(
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
    void destroy() override;

private:
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
};

} // namespace gl

} // namespace tmig