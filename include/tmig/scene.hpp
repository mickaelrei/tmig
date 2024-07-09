#pragma once

#include <vector>
#include <memory>

#include "tmig/entity.hpp"
#include "tmig/camera.hpp"
#include "tmig/utils/skybox.hpp"

namespace tmig {

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void addEntity(const std::shared_ptr<Entity> &entity);

    virtual void update(float dt) const;
    virtual void render(const gl::Shader &shader) const;

    Camera camera;
    utils::Skybox skybox;

    std::vector<std::shared_ptr<Entity>> entities;
protected:
};

} // namespace tmig