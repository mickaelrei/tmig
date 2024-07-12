#pragma once

#include <vector>
#include <memory>

#include "tmig/entity.hpp"
#include "tmig/camera.hpp"
#include "tmig/utils/skybox.hpp"

namespace tmig {

/// @brief Scene that holds a list of graphic entities
class Scene {
public:
    /// @brief Add new entity to scene
    /// @param entity entity to be added
    void addEntity(const std::shared_ptr<Entity> &entity);

    /// @brief Update scene (could be useful for an inherited scene class with special behavior)
    /// @param dt delta time
    virtual void update(float dt) const;

    /// @brief Render scene
    /// @param shader scene shader
    virtual void render(const gl::Shader &shader) const;

    /// @brief Current camera
    Camera camera;

    /// @brief Visual skybox
    utils::Skybox skybox;

    /// @brief List of entities currently in scene
    std::vector<std::shared_ptr<Entity>> entities;
};

} // namespace tmig