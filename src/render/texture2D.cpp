#include "stb/stb_image.h"
#include "glad/glad.h"

#include "tmig/render/texture2D.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

/// @brief Convert a `TextureFormat` into a sized OpenGL internal format
static GLenum toInternalFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8:       return GL_R8;
    case TextureFormat::RG8:      return GL_RG8;
    case TextureFormat::RGB8:     return GL_RGB8;
    case TextureFormat::RGBA8:    return GL_RGBA8;

    case TextureFormat::R16F:     return GL_R16F;
    case TextureFormat::RG16F:    return GL_RG16F;
    case TextureFormat::RGB16F:   return GL_RGB16F;
    case TextureFormat::RGBA16F:  return GL_RGBA16F;

    case TextureFormat::R32F:     return GL_R32F;
    case TextureFormat::RG32F:    return GL_RG32F;
    case TextureFormat::RGB32F:   return GL_RGB32F;
    case TextureFormat::RGBA32F:  return GL_RGBA32F;

    case TextureFormat::SRGB8:    return GL_SRGB8;
    case TextureFormat::SRGBA8:   return GL_SRGB8_ALPHA8;

    case TextureFormat::DEPTH24:  return GL_DEPTH_COMPONENT24;
    case TextureFormat::DEPTH32F: return GL_DEPTH_COMPONENT32F;
    }
    return 0;
}

/// @brief Convert a `TextureFormat` into a OpenGL data format
static GLenum toFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8:
    case TextureFormat::R16F:
    case TextureFormat::R32F:
        return GL_RED;

    case TextureFormat::RG8:
    case TextureFormat::RG16F:
    case TextureFormat::RG32F:
        return GL_RG;

    case TextureFormat::RGB8:
    case TextureFormat::RGB16F:
    case TextureFormat::RGB32F:
    case TextureFormat::SRGB8:
        return GL_RGB;

    case TextureFormat::RGBA8:
    case TextureFormat::RGBA16F:
    case TextureFormat::RGBA32F:
    case TextureFormat::SRGBA8:
        return GL_RGBA;

    case TextureFormat::DEPTH24:
    case TextureFormat::DEPTH32F:
        return GL_DEPTH_COMPONENT;
    }
    return 0;
}

/// @brief Convert a `TextureFormat` into a OpenGL data type
static GLenum toType(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8:
    case TextureFormat::RG8:
    case TextureFormat::RGB8:
    case TextureFormat::RGBA8:
    case TextureFormat::SRGB8:
    case TextureFormat::SRGBA8:
        return GL_UNSIGNED_BYTE;

    case TextureFormat::R16F:
    case TextureFormat::RG16F:
    case TextureFormat::RGB16F:
    case TextureFormat::RGBA16F:
        return GL_HALF_FLOAT;

    case TextureFormat::R32F:
    case TextureFormat::RG32F:
    case TextureFormat::RGB32F:
    case TextureFormat::RGBA32F:
    case TextureFormat::DEPTH32F:
        return GL_FLOAT;

    case TextureFormat::DEPTH24:
        return GL_UNSIGNED_INT;

    }
    return 0;
}

inline const char* toString(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:       return "R8";
        case TextureFormat::RG8:      return "RG8";
        case TextureFormat::RGB8:     return "RGB8";
        case TextureFormat::RGBA8:    return "RGBA8";

        case TextureFormat::R16F:     return "R16F";
        case TextureFormat::RG16F:    return "RG16F";
        case TextureFormat::RGB16F:   return "RGB16F";
        case TextureFormat::RGBA16F:  return "RGBA16F";

        case TextureFormat::R32F:     return "R32F";
        case TextureFormat::RG32F:    return "RG32F";
        case TextureFormat::RGB32F:   return "RGB32F";
        case TextureFormat::RGBA32F:  return "RGBA32F";

        case TextureFormat::SRGB8:    return "SRGB8";
        case TextureFormat::SRGBA8:   return "SRGB8_ALPHA8";

        case TextureFormat::DEPTH24:  return "DEPTH24";
        case TextureFormat::DEPTH32F: return "DEPTH32F";

        default:                      return "Unknown";
    }
}

Texture2D::Texture2D() {
    glCreateTextures(GL_TEXTURE_2D, 1, &_id); glCheckError();
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &_id); glCheckError();
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : _id{other._id},
      _width{other._width},
      _height{other._height},
      _internalFormat{other._internalFormat}
{
    other._id = 0;
    other._width = 0;
    other._height = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
    if (this != &other) {
        glDeleteTextures(1, &_id);

        _id = other._id;
        _width = other._width;
        _height = other._height;
        _internalFormat = other._internalFormat;

        other._id = 0;
        other._width = 0;
        other._height = 0;
    }
    return *this;
}

bool Texture2D::loadFromFile(const std::string& filename, bool flipY) {
    stbi_set_flip_vertically_on_load(flipY);
    int w, h, channels;
    unsigned char* data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
    if (!data) return false;

    // Get format; when loading from a file, we always treat it as 8-bit unsigned byte per channel
    TextureFormat format;
    switch (channels) {
        case 1: format = TextureFormat::R8; break;
        case 2: format = TextureFormat::RG8; break;
        case 3: format = TextureFormat::RGB8; break;
        case 4: format = TextureFormat::RGBA8; break;
        default:
            stbi_image_free(data);
            return false;
    }

    resize(w, h, format);
    setData(data);
    stbi_image_free(data);
    return true;
}

void Texture2D::setData(const void* data) {
    glTextureSubImage2D(_id, 0, 0, 0, _width, _height, toFormat(_internalFormat), toType(_internalFormat), data); glCheckError();

    // TODO: Let the user manually call a generateMipmaps() method rather than always doing it automatically
    glGenerateTextureMipmap(_id); glCheckError();
}

void Texture2D::resize(unsigned int width, unsigned int height, TextureFormat format) {
    _width = width;
    _height = height;
    _internalFormat = format;

    glTextureStorage2D(_id, 1, toInternalFormat(_internalFormat), width, height); glCheckError();
}

void Texture2D::bind(unsigned int unit) const {
    glBindTextureUnit(unit, _id); glCheckError();
}

void Texture2D::unbind(unsigned int unit) {
    glBindTextureUnit(unit, 0); glCheckError();
}

} // namespace tmig::render