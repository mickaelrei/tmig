#pragma once

#include "tmig/render/postprocessing/effect.hpp"
#include "tmig/render/framebuffer.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/texture2D.hpp"
#include "tmig/render/mesh.hpp"

namespace tmig::render::postprocessing {

/// @brief Configuration for the BlurEffect
struct BlurConfig {
    /// @brief Framebuffer width
    uint32_t width = 1280;

    /// @brief Framebuffer height
    uint32_t height = 720;
};

/// @brief Applies a Gaussian blur effect to a texture
class BlurEffect : public Effect {
public:
    /// @brief Constructor with configuration
    BlurEffect(const BlurConfig& config = {});

    /// @brief Destructor
    ~BlurEffect();
    
    /// @brief Set the scale for sampling offsets from the texture. Larger values create a wider, more diffuse blur
    /// @note By default it is 1.0f
    void setOffsetScale(float offsetScale);
    
    /// @brief Get current offset scale
    float getOffsetScale() const { return offsetScale; }

    /// @brief Applies the blur effect to the input texture
    virtual const Texture2D& apply(const Texture2D& input) override;

    /// @brief Number of blur iterations. More iterations create a smoother, heavier blur but cost more performance
    uint32_t blurIterations = 5;

protected:
    // Parameters
    float offsetScale = 1.0f;

    // Ping-pong framebuffers
    Framebuffer blurFramebuffers[2];

    // Blur shader
    ShaderProgram blurShader;

    // Ping-pong textures
    Texture2D blurTextures[2];

    // Data for the screen quad for intermediate renders
    struct quadVert {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    DataBuffer<quadVert>* _vertBuffer;
    DataBuffer<uint32_t>* _indexBuffer;
    Mesh<quadVert> _screenQuad;
};

} // namespace tmig::render::postprocessing