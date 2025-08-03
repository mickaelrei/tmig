#include <vector>
#include <unordered_set>

#include "glad/glad.h"

#include "tmig/render/framebuffer.hpp"
#include "tmig/util/debug.hpp"

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
    util::debugPrint("Created FBO: %u\n", _id);
}

Framebuffer::~Framebuffer() {
    if (_id != 0) {
        util::debugPrint("Deleting FBO: %u\n", _id);
        glDeleteFramebuffers(1, &_id);
    }
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

    // Attach draw buffers
    std::vector<GLenum> drawBuffers;
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
        glNamedFramebufferTexture(_id, attachPoint, attachment.texture->id(), 0);
        drawBuffers.push_back(attachPoint);
        _colorAttachments[index] = attachment;
    }

    // Tell which draw buffers we will be using on this framebuffer
    if (!drawBuffers.empty()) {
        glNamedFramebufferDrawBuffers(_id, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    } else {
        glNamedFramebufferDrawBuffer(_id, GL_NONE);
    }

    // Attach depth buffer if given
    if (config.depthAttachment.has_value()) {
        auto& attachment = config.depthAttachment.value();

        // Ensure valid texture and not duplicate
        if (!attachment.texture) return Status::NULL_ATTACHMENT;
        if (usedTextures.count(attachment.texture)) return Status::DUPLICATE_ATTACHMENT;
        usedTextures.insert(attachment.texture);

        // Attach the 
        GLenum attachPoint = depthFormatToAttachmentPoint(attachment.format);
        attachment.texture->resize(_width, _height, depthFormatToTextureFormat(attachment.format));
        glNamedFramebufferTexture(_id, attachPoint, attachment.texture->id(), 0);
        _depthAttachment = attachment;
    }

    // Check status
    GLenum status = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);
    return enumToStatus(status);
}

void Framebuffer::bind(const FramebufferBindOptions& options) const {
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