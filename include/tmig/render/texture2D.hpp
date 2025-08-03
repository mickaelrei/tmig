#pragma once

#include <string>
#include <cstdint>

#include "tmig/core/non_copyable.hpp"

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

    /// @brief Texture2D starts with this format until a call to `loadFromFile` or `resize` is made
    UNDEFINED,
};

/// @brief Represents the supported texture wrapping modes for textures when they are sampled outside
/// their bounds.
///
/// Texture wrapping modes define how textures behave when texture coordinates are outside the range [0, 1].
/// These modes determine how the texture is sampled at the edges and beyond.
enum class TextureWrap {
    /// @brief Repeat the texture infinitely in both directions
    REPEAT,

    /// @brief Mirror the texture on each repetition (odd repetitions are flipped)
    MIRRORED_REPEAT,

    /// @brief Clamp the texture coordinates to the edge of the texture
    CLAMP_TO_EDGE,
};

/// @brief Represents the supported texture filtering modes for when the texture is minified.
///
/// Texture minification occurs when the texture is viewed at smaller sizes. The filter determines how
/// the GPU samples the texture when its dimensions are reduced, such as when it is seen from a distance.
enum class TextureMinFilter {
    /// @brief Sample the nearest texel, no interpolation
    NEAREST,

    /// @brief Sample the texture with linear interpolation
    LINEAR,

    /// @brief Use the nearest mipmap level, and sample using the nearest texel
    NEAREST_MIPMAP_NEAREST,

    /// @brief Use the nearest mipmap level, and sample using linear interpolation
    LINEAR_MIPMAP_NEAREST,

    /// @brief Linearly interpolate between two mipmap levels, using the nearest texel
    NEAREST_MIPMAP_LINEAR,

    /// @brief Linearly interpolate between two mipmap levels, using linear interpolation for texels
    LINEAR_MIPMAP_LINEAR,
};

/// @brief Represents the supported texture filtering modes for when the texture is magnified.
///
/// Texture magnification occurs when the texture is viewed at larger sizes. The filter determines how
/// the GPU samples the texture when it needs to be upscaled.
enum class TextureMagFilter {
    /// @brief Sample the nearest texel, no interpolation
    NEAREST,

    /// @brief Sample the texture with linear interpolation
    LINEAR,
};

/// @brief Class representing a 2D texture, used for texturing objects in rendering
/// @note - This is a non-copyable class, meaning you cannot create a copy of it.
class Texture2D : protected core::NonCopyable {
public:
    /// @brief Default constructor
    Texture2D();

    /// @brief Destructor
    ~Texture2D();

    /// @brief Move constructor
    Texture2D(Texture2D&& other) noexcept;

    /// @brief Move assignment
    Texture2D& operator=(Texture2D&& other) noexcept;

    /// @brief Load from file
    /// @param path Path to file
    /// @param flipY whether should flip image vertically
    /// @note - This loads the texture data and sets the texture format based on the file format
    /// @note - Mipmap generation must be requested manually after loading the texture if desired.
    ///         This method does not automatically generate mipmaps
    bool loadFromFile(const std::string& path, bool flipY = true);

    /// @brief Set pixel data
    /// @param data Pointer to the texture data
    /// @note `data` will be interpreted as being in the same format as the current `format`,
    ///        so make sure it matches
    void setData(const void *data);

    /// @brief Resize texture
    /// @note - The `format` passed here will be used in further calls to `setData`, so make sure to
    ///       always match it
    /// @note - This will reallocate storage for the texture, including removing any existing mipmaps.
    ///         If mipmaps are desired, `generateMipmaps` must be called manually after resizing
    void resize(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);

    /// @brief Set texture wrap parameters
    /// @param wrapS Texture wrap mode for the S axis (horizontal)
    /// @param wrapT Texture wrap mode for the T axis (vertical)
    void setWrap(TextureWrap wrapS, TextureWrap wrapT);

    /// @brief Set texture filters
    /// @param minFilter Minification filter (used when the texture is scaled down)
    /// @param magFilter Magnification filter (used when the texture is scaled up)
    /// @note If using mipmap filters (e.g., `LINEAR_MIPMAP_NEAREST`), `generateMipmaps` must be called first.
    ///       Otherwise it throws an assertion
    void setFilter(TextureMinFilter minFilter, TextureMagFilter magFilter);

    /// @brief Generate texture mipmap levels
    /// @note This must be called manually to generate mipmaps. Mipmap generation is not automatic
    void generateMipmaps();

    /// @brief Bind to texture unit
    void bind(uint32_t unit = 0) const;

    /// @brief Unbind from texture unit
    static void unbind(uint32_t unit = 0);

    /// @brief Texture ID; used internally
    uint32_t id() const { return _id; }

    /// @brief Texture width
    uint32_t width() const { return _width; }

    /// @brief Texture height
    uint32_t height() const { return _height; }

    /// @brief Current format used internally
    TextureFormat format() const { return _internalFormat; }

private:
    /// @brief Texture OpenGL identifier
    uint32_t _id = 0;

    /// @brief Texture width in pixels
    uint32_t _width = 0;

    /// @brief Texture height in pixels
    uint32_t _height = 0;

    /// @brief Whether texture has mipmaps generated
    bool _hasMipmaps = false;

    /// @brief Current internal format
    TextureFormat _internalFormat = TextureFormat::UNDEFINED;
};

} // namespace tmig::render