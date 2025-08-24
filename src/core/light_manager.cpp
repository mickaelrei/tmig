#include "tmig/core/light_manager.hpp"

namespace tmig::core {

DirectionalLightHandle LightManager::addDirectionalLight(const render::DirectionalLight& light) {
    if (_lightsUBO.numDirectionalLights >= MAX_DIRECTIONAL_LIGHTS) {
        return DirectionalLightHandle(nullptr, 0);
    }

    _lightsUBO.directionalLights[_lightsUBO.numDirectionalLights++] = light;
    update();

    return DirectionalLightHandle(this, _lightsUBO.numDirectionalLights - 1);
}

PointLightHandle LightManager::addPointLight(const render::PointLight& light) {
    if (_lightsUBO.numPointLights >= MAX_POINT_LIGHTS) {
        return PointLightHandle(nullptr, 0);
    }
    
    _lightsUBO.pointLights[_lightsUBO.numPointLights++] = light;
    update();

    return PointLightHandle(this, _lightsUBO.numPointLights - 1);
}

SpotLightHandle LightManager::addSpotLight(const render::SpotLight& light) {
    if (_lightsUBO.numSpotLights >= MAX_SPOT_LIGHTS) {
        return SpotLightHandle(nullptr, 0);
    }

    _lightsUBO.spotLights[_lightsUBO.numSpotLights++] = light;
    update();

    return SpotLightHandle(this, _lightsUBO.numSpotLights - 1);
}

void LightManager::bindTo(uint32_t index) {
    ubo.bindTo(index);
}

void LightManager::update() {
    ubo.setData(_lightsUBO);
}

void DirectionalLightHandle::setDirection(const glm::vec3& dir) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.directionalLights[_index].direction = dir;
}

void DirectionalLightHandle::setColor(const glm::vec3& color) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.directionalLights[_index].color = color;
}

void DirectionalLightHandle::setIntensity(float intensity) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.directionalLights[_index].intensity = intensity;
}

void PointLightHandle::setPosition(const glm::vec3& pos) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.pointLights[_index].position = pos;
}

void PointLightHandle::setColor(const glm::vec3& color) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.pointLights[_index].color = color;
}

void PointLightHandle::setIntensity(float intensity) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.pointLights[_index].intensity = intensity;
}

void PointLightHandle::setConstant(float constant) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.pointLights[_index].constant = constant;
}

void PointLightHandle::setLinear(float linear) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.pointLights[_index].linear = linear;
}

void PointLightHandle::setQuadratic(float quadratic) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.pointLights[_index].quadratic = quadratic;
}

void SpotLightHandle::setPosition(const glm::vec3& pos) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].position = pos;
}

void SpotLightHandle::setDirection(const glm::vec3& dir) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].direction = dir;
}

void SpotLightHandle::setColor(const glm::vec3& color) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].color = color;
}

void SpotLightHandle::setIntensity(float intensity) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].intensity = intensity;
}

void SpotLightHandle::setCutOff(float cutOff) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].cutOff = cutOff;
}

void SpotLightHandle::setCutOffAngle(float cutOffAngle) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].cutOff = glm::cos(cutOffAngle);
}

void SpotLightHandle::setOuterCutOff(float outerCutOff) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].outerCutOff = outerCutOff;
}

void SpotLightHandle::setOuterCutOffAngle(float outerCutOffAngle) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].outerCutOff = glm::cos(outerCutOffAngle);
}

void SpotLightHandle::setConstant(float constant) {
    _manager->_lightsUBO.spotLights[_index].constant = constant;
    if (_manager == nullptr) return;
}

void SpotLightHandle::setLinear(float linear) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].linear = linear;
}

void SpotLightHandle::setQuadratic(float quadratic) {
    if (_manager == nullptr) return;
    _manager->_lightsUBO.spotLights[_index].quadratic = quadratic;
}


} // namespace tmig::core