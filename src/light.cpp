#include <stdexcept>

#include "tmig/light.hpp"

namespace tmig {

Light::Light(const glm::vec3 &color, float strength, Type type)
    : color{color},
      enabled{true},
      strength{strength},
      _type{type} {}

Light::Type Light::type() {
    return _type;
}

void Light::bind(const gl::Shader &shader, const std::string &prefix) const {
    std::string colorName = prefix + ".color";
    shader.setVec3(colorName, color);

    std::string enabledName = prefix + ".enabled";
    shader.setInt(enabledName, enabled);

    std::string strengthName = prefix + ".strength";
    shader.setFloat(strengthName, strength);
}

PointLight::PointLight(
    const glm::vec3 &color, float strength,
    const glm::vec3 &pos
)
    : Light::Light{color, strength, Light::Type::point},
      pos{pos} {}

void PointLight::bind(const gl::Shader &shader, const std::string &prefix) const {
    // Default bindings
    Light::bind(shader, prefix);

    std::string posName = prefix + ".pos";
    shader.setVec3(posName, pos);
}

DirectionalLight::DirectionalLight(
    const glm::vec3 &color, float strength,
    const glm::vec3 &dir
)
    : Light::Light{color, strength, Light::Type::directional},
      dir{dir} {}

void DirectionalLight::bind(const gl::Shader &shader, const std::string &prefix) const {
    // Default bindings
    Light::bind(shader, prefix);

    std::string dirName = prefix + ".dir";
    shader.setVec3(dirName, glm::normalize(dir));
}

SpotLight::SpotLight(
    const glm::vec3 &color, float strength,
    const glm::vec3 &pos,
    const glm::vec3 &dir,
    float cutoffAngle,
    float outerCutoffAngle
)
    : Light::Light{color, strength, Light::Type::spot},
      pos{pos},
      dir{dir},
      cutoffAngle{cutoffAngle},
      outerCutoffAngle{outerCutoffAngle} {}

void SpotLight::bind(const gl::Shader &shader, const std::string &prefix) const {
    // Default bindings
    Light::bind(shader, prefix);

    std::string posName = prefix + ".pos";
    shader.setVec3(posName, pos);

    std::string dirName = prefix + ".dir";
    shader.setVec3(dirName, glm::normalize(dir));

    std::string cutoffAngleName = prefix + ".cutoffAngle";
    shader.setFloat(cutoffAngleName, glm::cos(cutoffAngle));

    std::string outerCutoffAngleName = prefix + ".outerCutoffAngle";
    shader.setFloat(outerCutoffAngleName, glm::cos(outerCutoffAngle));
}

} // namespace tmig