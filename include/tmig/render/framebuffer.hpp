#pragma once

#include <ostream>
#include <unordered_map>
#include <memory>
#include <optional>

#include "tmig/core/non_copyable.hpp"
#include "tmig/render/texture2D.hpp"

namespace tmig::render {

/// @brief Represents the supported formats for depth and depth-stencil attachments in a framebuffer.
///
/// These formats define how depth (and optionally stencil) information is stored in the depth attachment
/// texture used by a framebuffer
enum class DepthAttachmentFormat {
   /// @brief 24-bit depth component
    DEPTH24,

    /// @brief 32-bit float depth component
    DEPTH32F,

    /// @brief 24-bit depth, 8-bit stencil combined component
    DEPTH24_STENCIL8,

    /// @brief 32-bit float depth, 8-bit stencil combined component
    DEPTH32F_STENCIL8,
};

/// @brief Describes a single color attachment to be used in a framebuffer.
///
/// This structure defines which texture will be used for a specific color attachment index,
/// and what format the framebuffer should expect for that attachment.
/// @note - The framebuffer does not own the texture and will not delete it upon destruction
/// @note - The texture's current format will be ignored and replaced by the format defined here
struct FramebufferAttachment {
    /// @brief Pointer to the attached texture (must outlive the framebuffer)
    Texture2D* texture;

    /// @brief Format of the attachment
    TextureFormat format;
};

/// @brief Describes the depth (or depth-stencil) attachment used in a framebuffer.
///
/// This structure specifies a single depth attachment texture and the format it is expected to use.
/// @note - The framebuffer does not own the texture and will not delete it upon destruction
/// @note - The texture's current format will be ignored and replaced by the format defined here
struct FramebufferDepthAttachment {
    /// @brief Pointer to the depth texture (must outlive the framebuffer)
    Texture2D* texture;

    /// @brief Format of the depth (or depth-stencil) attachment
    DepthAttachmentFormat format;
};

/// @brief Configuration used to initialize a framebuffer.
///
/// This structure describes the dimensions and attachments of a framebuffer.
/// All attachments will be resized to match dimensions with the framebuffer.
/// Color attachments are indexed by their key on the attachment map.
///
/// @note - Must have at least one color attachment
/// @note - The framebuffer does not take ownership of the provided textures.
struct FramebufferConfig {
    /// @brief Width of the framebuffer and all attachments in pixels
    uint32_t width = 0;

    /// @brief Height of the framebuffer and all attachments in pixels
    uint32_t height = 0;

    /// @brief Mapping from color attachment index to attachment info. Each entry defines one color
    /// output target.
    std::unordered_map<uint32_t, FramebufferAttachment> colorAttachments;

    /// @brief Optional depth (or depth-stencil) attachment. If not provided, the framebuffer will not
    /// have depth or stencil capability.
    std::optional<FramebufferDepthAttachment> depthAttachment = std::nullopt;
};

/// @brief Options for binding a framebuffer for rendering.
///
/// Controls automatic viewport resizing and which buffers should be cleared when binding.
struct FramebufferBindOptions {
    ///< Whether to set the viewport to match the framebuffer size
    bool setViewport = true;

    /// @brief Whether to clear the color buffers on bind
    bool clearColor = true;

    /// @brief Whether to clear the stencil buffer on bind
    bool clearStencil = true;

    /// @brief Whether to clear the depth buffer on bind
    bool clearDepth = true;
};

/// @brief Class representing an OpenGL framebuffer object
///
/// A framebuffer allows off-screen rendering to textures for post-processing, shadow mapping,
/// deferred shading, and more. Attachments (color, depth, and stencil) are provided externally
/// and are not owned or deleted by the framebuffer.
class Framebuffer {
public:
    /// @brief Construct an empty framebuffer.
    /// The framebuffer must be explicitly set up using `setup()` before use.
    Framebuffer();

    /// @brief Destroy the framebuffer object.
    /// Note that attached textures are not deleted.
    ~Framebuffer();

    /// @brief Possible status values returned by a call to `setup`
    ///
    /// These statuses correspond to OpenGL's framebuffer completeness states,
    /// plus a few custom error codes for misconfiguration.
    enum class Status {
        COMPLETE,                 // The framebuffer is complete and ready for use
        UNDEFINED,                // Default framebuffer is incomplete (usually unused here)
        INCOMPLETE_ATTACHMENT,    // One or more attachments are incomplete
        MISSING_ATTACHMENT,       // No attachments were provided
        INCOMPLETE_DRAW_BUFFER,   // Draw buffer points to an invalid attachment
        INCOMPLETE_READ_BUFFER,   // Read buffer points to an invalid attachment
        UNSUPPORTED,              // Combination of attachments is not supported by the driver
        INCOMPLETE_MULTISAMPLE,   // Multisample settings are inconsistent between attachments
        LAYER_TARGETS,            // Layered rendering targets mismatch
        NULL_ATTACHMENT,          // A provided attachment is null
        DUPLICATE_ATTACHMENT,     // Multiple attachments point to the same texture
        INVALID_DEPTH_ATTACHMENT, // Depth attachment format is invalid or mismatched
        INVALID_COLOR_INDEX,      // Color attachment index is out of bounds
        UNKNOWN,                  // Unknown or unhandled OpenGL error
    };

    /// @brief Set up the framebuffer using the provided configuration. Must be called before usage
    /// @note This function should only be called once unless framebuffer reconfiguration is required.
    ///       Repeated calls will override the current setup and may invalidate previously attached textures.
    Status setup(const FramebufferConfig& config);

    /// @brief Bind the framebuffer for rendering
    void bind(const FramebufferBindOptions& options = {}) const;

    /// @brief Resize the framebuffer's dimensions
    /// @param width New width in pixels
    /// @param height New height in pixels
    void resize(uint32_t width, uint32_t height);

    /// @brief Current framebuffer width
    uint32_t width() const { return _width; }

    /// @brief Current framebuffer height
    uint32_t height() const { return _height; }

    /// @brief Bind the default (screen) framebuffer.
    ///
    /// This binds OpenGL's default framebuffer (usually the window surface).
    /// Useful for restoring rendering to the screen after off-screen passes.
    static void bindDefault(uint32_t width, uint32_t height, const FramebufferBindOptions& options = {});

private:
    /// @brief OpenGL ID of the framebuffer
    uint32_t _id = 0;

    /// @brief Width in pixels
    uint32_t _width = 0;

    /// @brief Height in pixels
    uint32_t _height = 0;

    /// @brief Active color attachments of the framebuffer
    std::unordered_map<uint32_t, FramebufferAttachment> _colorAttachments;

    /// @brief Optional depth or depth-stencil attachment
    std::optional<FramebufferDepthAttachment> _depthAttachment = std::nullopt;
};

/// @brief Transforms a `Framebuffer::Status` into a displayable string
inline const char* toString(Framebuffer::Status status) {
    switch (status) {
    case Framebuffer::Status::COMPLETE:                 return "COMPLETE";
    case Framebuffer::Status::UNDEFINED:                return "UNDEFINED";
    case Framebuffer::Status::INCOMPLETE_ATTACHMENT:    return "INCOMPLETE_ATTACHMENT";
    case Framebuffer::Status::MISSING_ATTACHMENT:       return "MISSING_ATTACHMENT";
    case Framebuffer::Status::INCOMPLETE_DRAW_BUFFER:   return "INCOMPLETE_DRAW_BUFFER";
    case Framebuffer::Status::INCOMPLETE_READ_BUFFER:   return "INCOMPLETE_READ_BUFFER";
    case Framebuffer::Status::UNSUPPORTED:              return "UNSUPPORTED";
    case Framebuffer::Status::INCOMPLETE_MULTISAMPLE:   return "INCOMPLETE_MULTISAMPLE";
    case Framebuffer::Status::LAYER_TARGETS:            return "LAYER_TARGETS";
    case Framebuffer::Status::NULL_ATTACHMENT:          return "NULL_ATTACHMENT";
    case Framebuffer::Status::DUPLICATE_ATTACHMENT:     return "DUPLICATE_ATTACHMENT";
    case Framebuffer::Status::INVALID_DEPTH_ATTACHMENT: return "INVALID_DEPTH_ATTACHMENT";
    case Framebuffer::Status::INVALID_COLOR_INDEX:      return "INVALID_COLOR_INDEX";
    case Framebuffer::Status::UNKNOWN:                  return "UNKNOWN";
    default: return "INVALID_STATUS";
    }
}

/// @brief Operator << on ostream and `Framebuffer::Status`; uses `toString`
inline std::ostream& operator<<(std::ostream& os, Framebuffer::Status status) {
    return os << toString(status);
}

} // namespace tmig::render