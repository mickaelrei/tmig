#pragma once

#include <vector>
#include <memory>

#include "tmig/render/light.hpp"
#include "tmig/render/uniform_buffer.hpp"

namespace tmig::core {

class LightManager;

/// @brief A handle/proxy for a DirectionalLight added into a LightManager
struct DirectionalLightHandle {
public:
    /// @brief Sets the direction the light is pointing
    void setDirection(const glm::vec3& dir);
    
    /// @brief Sets the color of the light
    void setColor(const glm::vec3& color);
    
    /// @brief Sets the light intensity
    void setIntensity(float intensity);

    /// @brief Whether this handle is valid for usage
    bool isValid() const { return _manager != nullptr; }

private:
    /// @brief Private constructor only LightManager can access
    DirectionalLightHandle(LightManager* manager, uint32_t index) : _manager{manager}, _index{index} {}

    LightManager* _manager;
    uint32_t _index;

    friend class LightManager;
};

struct PointLightHandle {
public:
    /// @brief Sets the position of the light in world space
    void setPosition(const glm::vec3& pos);
    
    /// @brief Sets the color of the light
    void setColor(const glm::vec3& color);
    
    /// @brief Sets the light intensity
    void setIntensity(float intensity);
    
    /// @brief Sets the constant term for the light's attenuation formula
    void setConstant(float constant);
    
    /// @brief Sets the linear term for the light's attenuation formula
    void setLinear(float linear);
    
    /// @brief Sets the quadratic term for the light's attenuation formula
    void setQuadratic(float quadratic);

    /// @brief Whether this handle is valid for usage
    bool isValid() const { return _manager != nullptr; }

private:
    /// @brief Private constructor only LightManager can access
    PointLightHandle(LightManager* manager, uint32_t index) : _manager{manager}, _index{index} {}

    LightManager* _manager;
    uint32_t _index;

    friend class LightManager;
};

struct SpotLightHandle {
public:
    /// @brief Sets the position of the light in world space
    void setPosition(const glm::vec3& pos);
    
    /// @brief Sets the direction the light is pointing
    void setDirection(const glm::vec3& dir);
    
    /// @brief Sets the color of the light
    void setColor(const glm::vec3& color);
    
    /// @brief Sets the light intensity
    void setIntensity(float intensity);
    
    /// @brief Sets the inner cone angle (cosine value)
    void setCutOff(float cutOff);
    
    /// @brief Sets the inner cone angle in radians
    void setCutOffAngle(float cutOffAngle);
    
    /// @brief Sets the outer cone angle (cosine value) for a soft edge
    void setOuterCutOff(float outerCutOff);
    
    /// @brief Sets the outer cone angle in radians for a soft edge
    void setOuterCutOffAngle(float outerCutOffAngle);
    
    /// @brief Sets the constant term for the light's attenuation formula
    void setConstant(float constant);
    
    /// @brief Sets the linear term for the light's attenuation formula
    void setLinear(float linear);
    
    /// @brief Sets the quadratic term for the light's attenuation formula
    void setQuadratic(float quadratic);

    /// @brief Whether this handle is valid for usage
    bool isValid() const { return _manager != nullptr; }

private:
    /// @brief Private constructor only LightManager can access
    SpotLightHandle(LightManager* manager, uint32_t index) : _manager{manager}, _index{index} {}

    LightManager* _manager;
    uint32_t _index;

    friend class LightManager;
};

/// @brief Maximum number of directional lights supported in the UBO
static const int MAX_DIRECTIONAL_LIGHTS = 64;

/// @brief Maximum number of point lights supported in the UBO
static const int MAX_POINT_LIGHTS = 64;

/// @brief Maximum number of spotlights supported in the UBO
static const int MAX_SPOT_LIGHTS = 32;

/// @brief Data structure matching the layout of the lights UBO in shaders
/// correctly aligned according to std140 layout rules for uniform blocks
struct LightsUBO {
    /// @brief Array to hold all directional lights in the scene
    tmig::render::DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];

    /// @brief Array to hold all point lights in the scene
    tmig::render::PointLight pointLights[MAX_POINT_LIGHTS];

    /// @brief Array to hold all spot lights in the scene
    tmig::render::SpotLight spotLights[MAX_SPOT_LIGHTS];
    
    /// @brief The number of active directional lights currently in the array
    int numDirectionalLights;

    /// @brief The number of active point lights currently in the array
    int numPointLights;

    /// @brief The number of active spot lights currently in the array
    int numSpotLights;
};

/// @brief Manages a list of lights and syncs them with the GPu
///
/// This class provides a centralized system for handling lights. Adding a light
/// returns a handle (e.g., SpotLightHandle), which is used to modify the light's
/// properties
///
/// @note After all modifications are made for a frame, you must call `update()`
/// to upload the changes to the GPU
class LightManager {
public:
    /// @brief Adds a new directional light
    /// @return A handle to the newly added light
    DirectionalLightHandle addDirectionalLight(const render::DirectionalLight& light = {});

    /// @brief Add a new point light
    /// @return A handle to the newly added light
    PointLightHandle addPointLight(const render::PointLight& light = {});

    /// @brief Add a new spot light
    /// @return A handle to the newly added light
    SpotLightHandle addSpotLight(const render::SpotLight& light = {});

    /// @brief Bind light UniformBuffer to index
    void bindTo(uint32_t index);

    /// @brief Uploads the current state of all lights to the GPU
    /// @note Call this once per frame after making any modifications to the lights
    void update();

private:
    LightsUBO _lightsUBO{};
    render::UniformBuffer<LightsUBO> ubo;

    friend struct DirectionalLightHandle;
    friend struct PointLightHandle;
    friend struct SpotLightHandle;
};

} // namespace tmig::core