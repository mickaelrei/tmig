#include "scene.hpp"

namespace tmig {

void Scene::addEntity(const std::shared_ptr<Entity> &entity)
{
    entities.push_back(entity);
}

void Scene::setProjection(const glm::mat4 &projection) const
{
    // Set for skybox
    skybox.setProjectionMatrix(projection);

    // Set for all entities
    for (const auto &entity : entities)
    {
        entity->setProjectionMatrix(projection);
    }
}

void Scene::update(float dt) const
{
    (void)dt;

    // Set view matrix
    auto view = camera.getViewMatrix();

    skybox.setViewMatrix(view);
    for (const auto &entity : entities)
    {
        entity->setViewMatrix(view);
    }
}

void Scene::render() const
{
    // Render skybox first
    skybox.draw();

    // Draw all entities
    for (const auto &entity : entities)
    {
        entity->draw();
    }
}

} // namespace tmig
