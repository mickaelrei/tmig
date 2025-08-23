#include "tmig/render/postprocessing/effect.hpp"
#include "tmig/render/framebuffer.hpp"
#include "tmig/render/shader.hpp"
#include "tmig/render/mesh.hpp"

namespace tmig::render::postprocessing {

/// @brief Struct passed on BloomEffect constructor for configuration
struct BloomConfig {
    /// @brief Width of the bright pass framebuffer. Should be the window size or larger
    uint32_t brightPassWidth = 1920;

    /// @brief Height of the bright pass framebuffer. Should be the window size or larger
    uint32_t brightPassheight = 1080;

    /// @brief Width of the blur framebuffer. Quite expensive so should be smaller
    uint32_t blurWidth = 800;

    /// @brief Height of the blur framebuffer. Quite expensive so should be smaller
    uint32_t blurheight = 800;

    /// @brief Width of the output framebuffer. Very fast so could be very big
    uint32_t outputWidth = 2160;

    /// @brief Height of the output framebuffer. Very fast so could be very big
    uint32_t outputheight = 1440;
};

/// @brief Class representing a bloom effect for post-processing in a rendering pipeline.
/// 
/// The bloom effect simulates the phenomenon where bright light sources appear to bleed beyond
/// their boundaries, creating a glow. The effect operates in the following steps:
///
/// 1. Extract the excess (bright) light from the input texture and render it to a separate texture
///
/// 2. Apply a blur to the extracted excess light texture to create a soft glow
///
/// 3. Combine the blurred excess light texture with the original scene to produce the final image
class BloomEffect : public Effect {
public:
    /// @brief Constructor with configuration
    BloomEffect(const BloomConfig& config = {});

    /// @brief Destructor
    ~BloomEffect();
    
    /// @brief Set bright excess threshold. Fragments with brightness above this value
    /// will contribute to the bloom effect
    /// @note By default it is 1.5f
    void setThreshold(float threshold);
    
    /// @brief Get current bright excess threshold
    float getThreshold() const { return threshold; }

    /// @brief Set the scale for sampling offsets from the texture on the blur pass
    /// @note By default it is 1.5f
    void setOffsetScale(float offsetScale);
    
    /// @brief Get current offset scale
    float getOffsetScale() const { return offsetScale; }

    /// @brief Set the strength of the bloom when applied on the final output
    /// @note By default it is 0.5f
    void setStrength(float strength);
    
    /// @brief Get current offset scale
    float getStrength() const { return strength; }
    
    virtual const Texture2D& apply(const Texture2D& input) override;

    /// @brief Number of iterations on each axis when applying horizontal-vertical blur
    uint32_t blurIterations = 5;

protected:
    float threshold = 1.5f;            // Brightness threshold
    float offsetScale = 1.5f;          // Blur texture sample offset scale
    float strength = 0.5f;             // Bloom strength

    Framebuffer brightPassFramebuffer; // Bright excess pass framebuffer
    Framebuffer blurFramebuffers[2];   // Ping-pong blur framebuffers
    Framebuffer outputFramebuffer;     // Output framebuffer

    ShaderProgram brightPassShader;    // Bright excess pass shader
    ShaderProgram blurShader;          // Blur shader
    ShaderProgram outputShader;        // Output shader

    Texture2D brightPassTexture;       // Bright excess pass texture
    Texture2D blurTextures[2];         // Blur textures
    Texture2D outputTexture;           // Output texture

    struct quadVert {                  // Vertex type for screen quad
        glm::vec3 pos;
        glm::vec2 uv;
    };

    DataBuffer<quadVert>* vertBuffer;  // screen quad vertex buffer
    DataBuffer<uint32_t>* indexBuffer; // screen quad index buffer
    Mesh<quadVert> screenQuad;         // screen quad mesh
};

} // namespace tmig::render::postprocessing