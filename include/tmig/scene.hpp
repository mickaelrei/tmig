#pragma once

#include <vector>
#include <memory>

#include "tmig/entity.hpp"
#include "tmig/camera.hpp"
#include "tmig/light.hpp"
#include "tmig/utils/skybox.hpp"

namespace tmig {

/// @brief Scene that holds a list of graphic entities
class Scene {
public:
    /// @brief Add new entity to scene
    /// @param entity entity to be added
    void addEntity(const std::shared_ptr<Entity> &entity);

    /// @brief Add new light to scene
    /// @param light light to be added
    void addLight(const std::shared_ptr<Light> &light);

    /// @brief Set shader for scene rendering
    /// @param shader shader to be used
    void setShader(const gl::Shader &shader);

    /// @brief Update scene (could be useful for an inherited scene class with special behavior)
    /// @param dt delta time
    virtual void update(float dt);

    /// @brief Render scene
    virtual void render() const;

    /// @brief Current camera
    Camera camera;

    /// @brief Visual skybox
    utils::Skybox skybox;

    /// @brief Whether should render scene skybox
    bool renderSkybox = true;

    /// @brief Color used to clear color buffer
    glm::vec4 clearColor;

    /// @brief List of entities currently in scene
    std::vector<std::shared_ptr<Entity>> entities;

    /// @brief List of lights currently in scene
    std::vector<std::shared_ptr<Light>> lights;

protected:
    /// @brief Current shader for rendering the scene
    gl::Shader shader;
};

} // namespace tmig