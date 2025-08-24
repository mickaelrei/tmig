#pragma once

#include <glm/glm.hpp>

namespace tmig::render {

/// @brief A light source with parallel rays, such as sunlight
/// @note This struct is properly aligned according to the std140 layout for transferring to the GPU
struct DirectionalLight {
    alignas(16) glm::vec3 direction = glm::vec3{0.0f, 1.0f, 0.0f};
    alignas(16) glm::vec3 color = glm::vec3{1.0f};
    float intensity = 1.0f;
};

/// @brief A light source emitting light in all directions from a single point, such as a light bulb
/// @note This struct is properly aligned according to the std140 layout for transferring to the GPU
struct PointLight {
    alignas(16) glm::vec3 position = glm::vec3{0.0f};
    alignas(16) glm::vec3 color = glm::vec3{1.0f};
    float intensity = 1.0f;
    float constant = 1.0f;
    float linear = 0.1f;
    float quadratic = 0.03f;
};

/// @brief A light source emitting light in a cone-shaped beam, such as a flashlight
/// @note This struct is properly aligned according to the std140 layout for transferring to the GPU
struct SpotLight {
    alignas(16) glm::vec3 position = glm::vec3{0.0f};
    alignas(16) glm::vec3 direction = glm::vec3{0.0f, 1.0f, 0.0f};
    alignas(16) glm::vec3 color = glm::vec3{1.0f};
    float intensity = 1.0f;
    float cutOff = 0.866f;
    float outerCutOff = 0.82f;
    float constant = 1.0f;
    float linear = 0.1f;
    float quadratic = 0.03f;
};

} // namespace tmig::render