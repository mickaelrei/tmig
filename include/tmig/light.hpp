#pragma once

#include <glm/glm.hpp>

#include "tmig/gl/shader.hpp"

namespace tmig {

// TODO: Maybe add a diffuse and/or specular strenght to lights? This way
//       we could control how much each light influences the scene

/// @brief Base class used to represent a light in a scene
class Light {
public:
    /// @brief Types of light casters
    enum class Type {
        point,
        directional,
        spot
    };

    /// @brief Light color
    glm::vec3 color;

    /// @brief Whether light is enabled
    bool enabled;

    /// @brief Binds this light's data at a shader
    /// @param shader shader to bind info
    /// @param prefix uniform name prefix such as "lights[0]"
    virtual void bind(const gl::Shader &shader, const std::string &prefix) const;

    /// @brief Light caster type
    /// @return this light's type
    Type type();

protected:
    /// @brief Constructor
    /// @param pos light color
    /// @param enabled whether light is enabled 
    Light(const glm::vec3 &color, Type type);

    /// @brief Caster type
    Type _type;
};

/// @brief Light type where a single point emits light in all directions within a radius
class PointLight : public Light {
public:
    /// @brief Constructor
    /// @param color light color
    /// @param enabled whether light is enabled
    PointLight(
        const glm::vec3 &color,
        const glm::vec3 &pos = glm::vec3{0.0f}
    );

    /// @brief Light position in scene
    glm::vec3 pos;

    void bind(const gl::Shader &shader, const std::string &prefix) const override;
};

/// @brief Light type where light is emitted globally in a single direction
class DirectionalLight : public Light {
public:
    /// @brief Constructor
    /// @param color light color
    /// @param enabled whether light is enabled
    /// @param dir light direction
    DirectionalLight(
        const glm::vec3 &color,
        const glm::vec3 &dir = glm::vec3{0.0f, 0.0f, -1.0f}
    );

    /// @brief Light direction
    glm::vec3 dir;

    void bind(const gl::Shader &shader, const std::string &prefix) const override;
};

/// @brief Light type where light is emitted from a position in a direction with a cut-off angle
class SpotLight : public Light {
public:
    /// @brief Constructor
    /// @param color light color
    /// @param enabled whether light is enabled
    SpotLight(
        const glm::vec3 &color,
        const glm::vec3 &pos = glm::vec3{0.0f},
        const glm::vec3 &dir = glm::vec3{0.0f, 0.0f, -1.0f},
        float cutoffAngle = 0.0f,
        float outerCutoffAngle = 0.0f
    );

    /// @brief Light position in scene
    glm::vec3 pos;

    /// @brief Light direction
    glm::vec3 dir;

    /// @brief Cut-off angle
    float cutoffAngle;

    /// @brief Outer cut-off angle
    float outerCutoffAngle;

    void bind(const gl::Shader &shader, const std::string &prefix) const override;
};

} // namespace tmig