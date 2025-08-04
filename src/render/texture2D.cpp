#include <stdexcept>
#include <cassert>

#include "stb/stb_image.h"
#include "glad/glad.h"

#include "tmig/render/texture2D.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

bool isMipmapFilter(TextureMinFilter filter) {
    switch (filter) {
    case TextureMinFilter::NEAREST_MIPMAP_NEAREST:
    case TextureMinFilter::LINEAR_MIPMAP_NEAREST:
    case TextureMinFilter::NEAREST_MIPMAP_LINEAR:
    case TextureMinFilter::LINEAR_MIPMAP_LINEAR:
        return true;
    default:
        return false;
    }
}

/// @brief Convert a `TextureFormat` into a sized OpenGL internal format
static GLenum toInternalFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8:                return GL_R8;
    case TextureFormat::RG8:               return GL_RG8;
    case TextureFormat::RGB8:              return GL_RGB8;
    case TextureFormat::RGBA8:             return GL_RGBA8;

    case TextureFormat::R16F:              return GL_R16F;
    case TextureFormat::RG16F:             return GL_RG16F;
    case TextureFormat::RGB16F:            return GL_RGB16F;
    case TextureFormat::RGBA16F:           return GL_RGBA16F;

    case TextureFormat::R32F:              return GL_R32F;
    case TextureFormat::RG32F:             return GL_RG32F;
    case TextureFormat::RGB32F:            return GL_RGB32F;
    case TextureFormat::RGBA32F:           return GL_RGBA32F;

    case TextureFormat::SRGB8:             return GL_SRGB8;
    case TextureFormat::SRGBA8:            return GL_SRGB8_ALPHA8;

    case TextureFormat::DEPTH16:           return GL_DEPTH_COMPONENT16;
    case TextureFormat::DEPTH24:           return GL_DEPTH_COMPONENT24;
    case TextureFormat::DEPTH32F:          return GL_DEPTH_COMPONENT32F;

    case TextureFormat::STENCIL8:          return GL_STENCIL_INDEX8;

    case TextureFormat::DEPTH24_STENCIL8:  return GL_DEPTH24_STENCIL8;
    case TextureFormat::DEPTH32F_STENCIL8: return GL_DEPTH32F_STENCIL8;

    case TextureFormat::UNDEFINED:         return 0;
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

    case TextureFormat::DEPTH16:
    case TextureFormat::DEPTH24:
    case TextureFormat::DEPTH32F:
        return GL_DEPTH_COMPONENT;

    case TextureFormat::STENCIL8:
        return GL_STENCIL_INDEX;

    case TextureFormat::DEPTH24_STENCIL8:
    case TextureFormat::DEPTH32F_STENCIL8:
        return GL_DEPTH_STENCIL;

    case TextureFormat::UNDEFINED:
        return 0;
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
    case TextureFormat::STENCIL8:
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

    case TextureFormat::DEPTH16:
        return GL_UNSIGNED_SHORT;

    case TextureFormat::DEPTH24:
        return GL_UNSIGNED_INT;

    case TextureFormat::DEPTH24_STENCIL8:
        return GL_UNSIGNED_INT_24_8;

    case TextureFormat::DEPTH32F_STENCIL8:
        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

    case TextureFormat::UNDEFINED:
        return 0;
    }
    return 0;
}

inline const char* toString(TextureFormat format) {
    switch (format) {
    case TextureFormat::R8:                return "R8";
    case TextureFormat::RG8:               return "RG8";
    case TextureFormat::RGB8:              return "RGB8";
    case TextureFormat::RGBA8:             return "RGBA8";

    case TextureFormat::R16F:              return "R16F";
    case TextureFormat::RG16F:             return "RG16F";
    case TextureFormat::RGB16F:            return "RGB16F";
    case TextureFormat::RGBA16F:           return "RGBA16F";

    case TextureFormat::R32F:              return "R32F";
    case TextureFormat::RG32F:             return "RG32F";
    case TextureFormat::RGB32F:            return "RGB32F";
    case TextureFormat::RGBA32F:           return "RGBA32F";

    case TextureFormat::SRGB8:             return "SRGB8";
    case TextureFormat::SRGBA8:            return "SRGB8_ALPHA8";

    case TextureFormat::DEPTH16:           return "DEPTH16";
    case TextureFormat::DEPTH24:           return "DEPTH24";
    case TextureFormat::DEPTH32F:          return "DEPTH32F";

    case TextureFormat::STENCIL8:          return "STENCIL8";

    case TextureFormat::DEPTH24_STENCIL8:  return "DEPTH24_STENCIL8";
    case TextureFormat::DEPTH32F_STENCIL8: return "DEPTH32F_STENCIL8";
    case TextureFormat::UNDEFINED:         return "UNDEFINED";
    }

    return "Unknown";
}

static GLenum toGL(TextureWrapMode wrap) {
    switch (wrap) {
        case TextureWrapMode::REPEAT:          return GL_REPEAT;
        case TextureWrapMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case TextureWrapMode::CLAMP_TO_EDGE:   return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
        default:                           return GL_REPEAT;
    }
}

static GLenum toGL(TextureMinFilter filter) {
    switch (filter) {
        case TextureMinFilter::NEAREST:                return GL_NEAREST;
        case TextureMinFilter::LINEAR:                 return GL_LINEAR;
        case TextureMinFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureMinFilter::LINEAR_MIPMAP_NEAREST:  return GL_LINEAR_MIPMAP_NEAREST;
        case TextureMinFilter::NEAREST_MIPMAP_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
        case TextureMinFilter::LINEAR_MIPMAP_LINEAR:   return GL_LINEAR_MIPMAP_LINEAR;
        default:                                       return GL_NEAREST;
    }
}

static GLenum toGL(TextureMagFilter filter) {
    switch (filter) {
        case TextureMagFilter::NEAREST: return GL_NEAREST;
        case TextureMagFilter::LINEAR:  return GL_LINEAR;
        default:                        return GL_NEAREST;
    }
}

Texture2D::Texture2D() {
    glCreateTextures(GL_TEXTURE_2D, 1, &_id); glCheckError();
    util::debugPrint("Created Texture2D: %u\n", _id);
}

Texture2D::~Texture2D() {
    util::debugPrint("Deleting Texture2D: %u\n", _id);
    glDeleteTextures(1, &_id); glCheckError();
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : _id{other._id},
      _width{other._width},
      _height{other._height},
      _hasMipmaps{other._hasMipmaps},
      _internalFormat{other._internalFormat}
{
    other._id = 0;
    other._width = 0;
    other._height = 0;
    other._hasMipmaps = false;
    other._internalFormat = TextureFormat::UNDEFINED;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
    if (this != &other) {
        if (_id != 0) {
            glDeleteTextures(1, &_id);
        }

        _id = other._id;
        _width = other._width;
        _height = other._height;
        _hasMipmaps = other._hasMipmaps;
        _internalFormat = other._internalFormat;

        other._id = 0;
        other._width = 0;
        other._height = 0;
        other._hasMipmaps = false;
        other._internalFormat = TextureFormat::UNDEFINED;
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
    setData(data, format);
    stbi_image_free(data);
    return true;
}

void Texture2D::setData(const void* data, TextureFormat sourceFormat) {
#ifdef DEBUG
    // Validate that texture has been properly initialized
    if (_width == 0 || _height == 0 || _internalFormat == TextureFormat::UNDEFINED) {
        throw std::runtime_error{"[Texture2D::setData] Texture is not initialized"};
    }

    // Source format cannot be UNDEFINED
    if (sourceFormat == TextureFormat::UNDEFINED) {
        throw std::runtime_error{"[Texture2D::setData] TextureFormat::UNDEFINED isn't a valid source format"};
    }

    if (!Texture2D::isFormatCompatible(_internalFormat, sourceFormat)) {
        throw std::runtime_error("[Texture2D::setData] Incompatible source/internal texture format");
    }
#endif

    const auto format = toFormat(sourceFormat);
    const auto type = toType(sourceFormat);
    glTextureSubImage2D(_id, 0, 0, 0, _width, _height, format, type, data); glCheckError();
}

void Texture2D::resize(uint32_t width, uint32_t height, TextureFormat internalFormat) {
#ifdef DEBUG
    if (internalFormat == TextureFormat::UNDEFINED) {
        throw std::runtime_error{"[Texture2D::resize] TextureFormat::UNDEFINED isn't a valid internal format"};
    }
#endif

    // Reset mipmaps flag because resizing removes the existing mipmaps
    _hasMipmaps = false;

    _width = width;
    _height = height;
    _internalFormat = internalFormat;

    glTextureStorage2D(_id, 1, toInternalFormat(_internalFormat), width, height); glCheckError();
}

void Texture2D::setWrapS(TextureWrapMode wrap) {
    glTextureParameteri(_id, GL_TEXTURE_WRAP_S, toGL(wrap));
}

void Texture2D::setWrapT(TextureWrapMode wrap) {
    glTextureParameteri(_id, GL_TEXTURE_WRAP_T, toGL(wrap));
}

void Texture2D::setMinFilter(TextureMinFilter filter) {
    glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, toGL(filter));
}

void Texture2D::setMagFilter(TextureMagFilter filter) {
    glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, toGL(filter));
}

void Texture2D::setBorderColor(const glm::vec4& color) {
    glTextureParameterfv(_id, GL_TEXTURE_BORDER_COLOR, &color.x); glCheckError();
}

void Texture2D::generateMipmaps() {
    if (_hasMipmaps) return;

    _hasMipmaps = true;
    glGenerateTextureMipmap(_id); glCheckError();
}

void Texture2D::bind(uint32_t unit) const {
    glBindTextureUnit(unit, _id); glCheckError();
}

void Texture2D::unbind(uint32_t unit) {
    glBindTextureUnit(unit, 0); glCheckError();
}

bool Texture2D::isFormatCompatible(TextureFormat internal, TextureFormat source) {
    switch (internal) {
        // ---------- Unsigned Byte Formats ----------
        case TextureFormat::R8:
            return source == TextureFormat::R8;

        case TextureFormat::RG8:
            return source == TextureFormat::RG8;

        case TextureFormat::RGB8:
        case TextureFormat::SRGB8:
            return source == TextureFormat::RGB8;

        case TextureFormat::RGBA8:
        case TextureFormat::SRGBA8:
            return source == TextureFormat::RGBA8;

        // ---------- Half Float Formats ----------
        case TextureFormat::R16F:
            return source == TextureFormat::R16F || source == TextureFormat::R32F;

        case TextureFormat::RG16F:
            return source == TextureFormat::RG16F || source == TextureFormat::RG32F;

        case TextureFormat::RGB16F:
            return source == TextureFormat::RGB16F || source == TextureFormat::RGB32F;

        case TextureFormat::RGBA16F:
            return source == TextureFormat::RGBA16F || source == TextureFormat::RGBA32F;

        // ---------- Full Float Formats ----------
        case TextureFormat::R32F:
            return source == TextureFormat::R32F;

        case TextureFormat::RG32F:
            return source == TextureFormat::RG32F;

        case TextureFormat::RGB32F:
            return source == TextureFormat::RGB32F;

        case TextureFormat::RGBA32F:
            return source == TextureFormat::RGBA32F;

        // ---------- Depth Formats ----------
        case TextureFormat::DEPTH24:
            return source == TextureFormat::DEPTH24;

        case TextureFormat::DEPTH32F:
            return source == TextureFormat::DEPTH32F;

        // ---------- Depth-Stencil Formats ----------
        case TextureFormat::DEPTH24_STENCIL8:
            return source == TextureFormat::DEPTH24_STENCIL8;

        case TextureFormat::DEPTH32F_STENCIL8:
            return source == TextureFormat::DEPTH32F_STENCIL8;

        // ---------- Stencil Format ----------
        case TextureFormat::STENCIL8:
            return source == TextureFormat::STENCIL8;

        // ---------- Invalid ----------
        case TextureFormat::UNDEFINED:
        default:
            return false;
    }
}


} // namespace tmig::render