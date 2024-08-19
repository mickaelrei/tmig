#pragma once

#include <vector>
#include <memory>

#include "tmig/render/entity.hpp"
#include "tmig/render/camera.hpp"
#include "tmig/render/light.hpp"
#include "tmig/render/utils/skybox.hpp"

namespace tmig {

namespace render {

/// @brief Scene that holds a list of graphic entities
class Scene {
public:
    /// @brief Add new entity to scene
    /// @param entity entity to be added
    void addEntity(const std::shared_ptr<Entity> &entity);

    /// @brief Add new entity with transparency to scene
    /// @param entity entity to be added
    void addTransparentEntity(const std::shared_ptr<Entity> &entity);

    /// @brief Add new light to scene
    /// @param light light to be added
    void addLight(const std::shared_ptr<Light> &light);

    /// @brief Set shader for scene rendering
    /// @param shader shader to be used
    void setShader(const std::shared_ptr<gl::Shader> &shader);

    /// @brief Set projection matrix on shader
    /// @param viewportSize render viewport size
    void setProjection(const glm::ivec2 &viewportSize);

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

protected:
    /// @brief Current shader for rendering the scene
    std::shared_ptr<gl::Shader> shader;

    /// @brief List of entities currently in scene
    std::vector<std::shared_ptr<Entity>> entities;

    /// @brief List of transparent entities in scene
    std::vector<std::shared_ptr<Entity>> transparentEntities;

    /// @brief List of lights currently in scene
    std::vector<std::shared_ptr<Light>> lights;
};

} // namespace render

} // namespace tmig