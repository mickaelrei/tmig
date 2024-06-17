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

    void setProjection(const glm::mat4 &projection) const;

    virtual void update(float dt) const;
    virtual void render() const;

    Camera camera;
    utils::Skybox skybox;

protected:
    std::vector<std::shared_ptr<Entity>> entities;
};

} // namespace tmig