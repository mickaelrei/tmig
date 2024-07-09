#include "tmig/gl/gl.hpp"
#include "tmig/scene.hpp"

namespace tmig {

void Scene::addEntity(const std::shared_ptr<Entity> &entity)
{
    entities.push_back(entity);
}

void Scene::update(float dt) const
{
    (void)dt;
}

void Scene::render(const gl::Shader &shader) const
{
    // Render skybox first
    skybox.draw(gl::skyboxShader());

    // Draw all entities
    for (const auto &entity : entities)
    {
       entity->draw(shader);
    }
}

} // namespace tmig
