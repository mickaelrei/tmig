#pragma once

#include <string>

namespace tmig::render {

/// @brief Represents supported texture formats for GPU storage and shader sampling.
///
/// These formats define how texture data is stored in GPU memory. In shaders, missing channels
/// are filled as: `.r` = 0, `.g` = 0, `.b` = 0, `.a` = 1. sRGB formats auto-convert to linear.
enum class TextureFormat {
    // ---------------- Unsigned Byte Formats ----------------

    /// @brief 8-bit unsigned byte, single red channel
    R8,

    /// @brief 8-bit unsigned byte, two channels
    RG8,

    /// @brief 8-bit unsigned byte, three channels
    RGB8,

    /// @brief 8-bit unsigned byte, four channels
    RGBA8,

    // ---------------- Half Float Formats (16-bit float per channel) ----------------

    /// @brief 16-bit float, single channel
    R16F,

    /// @brief 16-bit float, two channels
    RG16F,

    /// @brief 16-bit float, three channels
    RGB16F,

    /// @brief 16-bit float, four channels
    RGBA16F,

    // ---------------- Full Float Formats (32-bit float per channel) ----------------

    /// @brief 32-bit float, single channel
    R32F,

    /// @brief 32-bit float, two channels
    RG32F,

    /// @brief 32-bit float, three channels
    RGB32F,

    /// @brief 32-bit float, four channels
    RGBA32F,

    // ---------------- sRGB Formats ----------------

    /// @brief 8-bit unsigned byte, RGB stored as sRGB
    SRGB8,

    /// @brief 8-bit unsigned byte, RGBA stored as sRGB color + linear alpha
    SRGBA8,

    // ---------------- Depth Formats ----------------

    /// @brief 24-bit depth component
    DEPTH24,

    /// @brief 32-bit float depth component
    DEPTH32F,
};

/// @brief Class representing a 2D texture, used for texturing objects in rendering
class Texture2D {
public:
    /// @brief Default constructor
    Texture2D();

    /// @brief Destructor
    ~Texture2D();

    /// @brief Copy constructor is disabled
    Texture2D(const Texture2D&) = delete;

    /// @brief Copy assignment is disabled
    Texture2D& operator=(const Texture2D&) = delete;

    /// @brief Move constructor
    Texture2D(Texture2D&& other) noexcept;

    /// @brief Move assignment
    Texture2D& operator=(Texture2D&& other) noexcept;

    /// @brief Load from file
    /// @param path Path to file
    /// @param flipY whether should flip image vertically
    /// @note This sets the `format` to be whatever format the file is
    bool loadFromFile(const std::string &path, bool flipY = true);

    /// @brief Set pixel data
    /// @note `data` will be interpreted as being in the same format as the current `format`, so make sure it matches
    void setData(const void *data);

    /// @brief Resize texture
    /// @note The `format` passed here will be used in further calls to `setData`, so make sure to always match it
    void resize(unsigned int width, unsigned int height, TextureFormat format = TextureFormat::RGBA8);

    /// @brief Bind to texture unit
    void bind(unsigned int unit = 0) const;

    /// @brief Unbind from texture unit
    static void unbind(unsigned int unit = 0);

    /// @brief Texture ID; used internally
    unsigned int id() const { return _id; }

    /// @brief Texture width
    unsigned int width() const { return _width; }

    /// @brief Texture height
    unsigned int height() const { return _height; }

    /// @brief Current format used internally
    TextureFormat format() const { return _internalFormat; }

private:
    unsigned int _id = 0;
    unsigned int _width = 0, _height = 0;
    TextureFormat _internalFormat;
};

} // namespace tmig::render