#include <vector>
#include <unordered_set>
#include <map>

#include "glad/glad.h"

#include "tmig/render/framebuffer.hpp"
#include "tmig/util/log.hpp"

namespace tmig::render {

static GLenum depthFormatToAttachmentPoint(DepthAttachmentFormat format) {
    switch (format) {
    case DepthAttachmentFormat::DEPTH24:
    case DepthAttachmentFormat::DEPTH32F:
        return GL_DEPTH_ATTACHMENT;

    case DepthAttachmentFormat::DEPTH24_STENCIL8:
    case DepthAttachmentFormat::DEPTH32F_STENCIL8:
        return GL_DEPTH_STENCIL_ATTACHMENT;

    default:
        return GL_COLOR_ATTACHMENT0;
    }
}

static TextureFormat depthFormatToTextureFormat(DepthAttachmentFormat format) {
    switch (format) {
    case DepthAttachmentFormat::DEPTH24:           return TextureFormat::DEPTH24;
    case DepthAttachmentFormat::DEPTH32F:          return TextureFormat::DEPTH32F;
    case DepthAttachmentFormat::DEPTH24_STENCIL8:  return TextureFormat::DEPTH24_STENCIL8;
    case DepthAttachmentFormat::DEPTH32F_STENCIL8: return TextureFormat::DEPTH32F_STENCIL8;
    }
    return TextureFormat::DEPTH24;
}

static Framebuffer::Status enumToStatus(GLenum e) {
    switch (e) {
    case GL_FRAMEBUFFER_COMPLETE:                      return Framebuffer::Status::COMPLETE;
    case GL_FRAMEBUFFER_UNDEFINED:                     return Framebuffer::Status::UNDEFINED;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return Framebuffer::Status::INCOMPLETE_ATTACHMENT;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return Framebuffer::Status::MISSING_ATTACHMENT;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        return Framebuffer::Status::INCOMPLETE_DRAW_BUFFER;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        return Framebuffer::Status::INCOMPLETE_READ_BUFFER;
    case GL_FRAMEBUFFER_UNSUPPORTED:                   return Framebuffer::Status::UNSUPPORTED;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        return Framebuffer::Status::INCOMPLETE_MULTISAMPLE;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      return Framebuffer::Status::LAYER_TARGETS;
    }
    return Framebuffer::Status::UNKNOWN;
}

Framebuffer::Framebuffer() {
    glCreateFramebuffers(1, &_id);
    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "Created FBO: %u\n", _id
    );
}

Framebuffer::~Framebuffer() {
    if (_id == 0) return;

    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "Deleting FBO: %u\n", _id
    );
    glDeleteFramebuffers(1, &_id);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : _id{other._id},
      _width{other._width},
      _height{other._height},
      _colorAttachments{std::move(other._colorAttachments)},
      _depthAttachment{std::move(other._depthAttachment)}
{
    other._id = 0;
    other._width = 0;
    other._height = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
        if (_id != 0) {
            glDeleteFramebuffers(1, &_id);
        }

        _id = other._id;
        _width = other._width;
        _height = other._height;
        _colorAttachments = std::move(other._colorAttachments);
        _depthAttachment = std::move(other._depthAttachment);

        other._id = 0;
        other._width = 0;
        other._height = 0;
    }
    return *this;
}

Framebuffer::Status Framebuffer::setup(const FramebufferConfig& config) {
    if (config.colorAttachments.empty()) {
        return Status::MISSING_ATTACHMENT;
    }

    // Reset state
    _width = config.width;
    _height = config.height;
    _colorAttachments.clear();
    _depthAttachment = std::nullopt;

    // Keep track of used textures to avoid duplicates
    std::unordered_set<Texture2D*> usedTextures;

    // Find the highest attachment index to determine the size of draw buffers array
    uint32_t maxIndex = 0;
    for (auto const& [index, _] : config.colorAttachments) {
        if (index > maxIndex) {
            maxIndex = index;
        }
    }
    std::vector<GLenum> drawBuffers(maxIndex + 1, GL_NONE);

    // Attach draw buffers
    for (auto& [index, attachment] : config.colorAttachments) {
        // Ensure valid texture and not duplicate
        if (!attachment.texture) return Status::NULL_ATTACHMENT;
        if (usedTextures.count(attachment.texture)) return Status::DUPLICATE_ATTACHMENT;
        if (index >= 8) return Status::INVALID_COLOR_INDEX; // TODO: 8 is not always the limit; query for the real limit
        usedTextures.insert(attachment.texture);

        // Resize and apply format
        attachment.texture->resize(_width, _height, attachment.format);

        // Attach to framebuffer
        GLenum attachPoint = GL_COLOR_ATTACHMENT0 + index;
        util::logMessage(
            util::LogCategory::ENGINE, util::LogSeverity::INFO,
            "glNamedFramebufferTexture(%u, GL_COLOR_ATTACHMENT%u, %u, 0)\n", _id, index, attachment.texture->id()
        );
        glNamedFramebufferTexture(_id, attachPoint, attachment.texture->id(), 0);
        drawBuffers[index] = attachPoint;
        _colorAttachments[index] = attachment;
    }

    // Tell which draw buffers we will be using on this framebuffer
    if (!drawBuffers.empty()) {
        util::logMessage(
            util::LogCategory::ENGINE, util::LogSeverity::INFO,
            "glNamedFramebufferDrawBuffers(%u, %zu, %p)\n", _id, drawBuffers.size(), drawBuffers.data()
        );
        glNamedFramebufferDrawBuffers(_id, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    } else {
        util::logMessage(
            util::LogCategory::ENGINE, util::LogSeverity::INFO,
            "glNamedFramebufferDrawBuffer(%u, GL_NONE)\n", _id
        );
        glNamedFramebufferDrawBuffer(_id, GL_NONE);
    }

    // Attach depth buffer if given
    if (config.depthAttachment.has_value()) {
        auto& attachment = config.depthAttachment.value();

        // Ensure valid texture and not duplicate
        if (!attachment.texture) return Status::NULL_ATTACHMENT;
        if (usedTextures.count(attachment.texture)) return Status::DUPLICATE_ATTACHMENT;
        usedTextures.insert(attachment.texture);

        // Resize and apply format
        attachment.texture->resize(_width, _height, depthFormatToTextureFormat(attachment.format));

        // Attach to framebuffer
        GLenum attachPoint = depthFormatToAttachmentPoint(attachment.format);
        glNamedFramebufferTexture(_id, attachPoint, attachment.texture->id(), 0);
        _depthAttachment = attachment;
    }

    // Check status
    GLenum statusGL = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);
    auto status = enumToStatus(statusGL);
    if (status == Status::COMPLETE) {
        setupCalled = true;
    }

    return status;
}

void Framebuffer::bind(const FramebufferBindOptions& options) const {
#ifdef DEBUG
    if (!setupCalled) {
        throw std::runtime_error{"[render::Framebuffer::bind] Tried to bind framebuffer not set up"};
    }
#endif

    glBindFramebuffer(GL_FRAMEBUFFER, _id);

    if (options.setViewport) {
        glViewport(0, 0, _width, _height);
    }

    GLbitfield clearMask = 0;
    if (options.clearColor)   clearMask |= GL_COLOR_BUFFER_BIT;
    if (options.clearDepth)   clearMask |= GL_DEPTH_BUFFER_BIT;
    if (options.clearStencil) clearMask |= GL_STENCIL_BUFFER_BIT;

    if (clearMask != 0) {
        glClear(clearMask);
    }
}

void Framebuffer::bindDefault(uint32_t width, uint32_t height, const FramebufferBindOptions& options) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (options.setViewport) {
        glViewport(0, 0, width, height);
    }

    GLbitfield clearMask = 0;
    if (options.clearColor)   clearMask |= GL_COLOR_BUFFER_BIT;
    if (options.clearDepth)   clearMask |= GL_DEPTH_BUFFER_BIT;
    if (options.clearStencil) clearMask |= GL_STENCIL_BUFFER_BIT;

    if (clearMask != 0) {
        glClear(clearMask);
    }
}

void Framebuffer::resize(uint32_t width, uint32_t height) {
    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "resizing fb to (%u, %u)\n", width, height
    );
    _width = width;
    _height = height;

    for (auto& [index, attachment] : _colorAttachments) {
        attachment.texture->resize(width, height, attachment.format);
    }

    if (_depthAttachment.has_value()) {
        auto& attachment = _depthAttachment.value();
        attachment.texture->resize(width, height, depthFormatToTextureFormat(attachment.format));
    }
}

} // namespace tmig::render