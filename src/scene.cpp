#include "tmig/utils/shaders.hpp"
#include "tmig/scene.hpp"

namespace tmig {

void Scene::addEntity(const std::shared_ptr<Entity> &entity)
{
    entities.push_back(entity);
}

void Scene::addLight(const std::shared_ptr<Light> &light)
{
    lights.push_back(light);
}

void Scene::setShader(const gl::Shader &shader) {
    Scene::shader = shader;
}

void Scene::update(float dt) {
    (void)dt;

    int numPointLights = 0;
    int numDirectionalLights = 0;
    int numSpotLights = 0;

    for (auto &light : lights) {
        std::string prefix;

        // Get prefix based on light type
        switch (light->type()) {
            case Light::Type::point:
                prefix = "pointLights[" + std::to_string(numPointLights++) + "]";
                break;
            case Light::Type::directional:
                prefix = "directionalLights[" + std::to_string(numDirectionalLights++) + "]";
                break;
            case Light::Type::spot:
                prefix = "spotLights[" + std::to_string(numSpotLights++) + "]";
                break;
            default:
                continue;
        }

        light->bind(shader, prefix);
    }

    shader.setInt("numPointLights", numPointLights);
    shader.setInt("numDirectionalLights", numDirectionalLights);
    shader.setInt("numSpotLights", numSpotLights);

    // TODO: This could be better encapsulated as a config
    shader.setFloat("ambientStrength", 0.2f);
    shader.setFloat("specularStrength", 0.5f);
}

void Scene::render() const
{
    // Render skybox first
    if (renderSkybox) {
        skybox.draw(*utils::skyboxShader());
    }

    // Draw all entities
    for (const auto &entity : entities)
    {
       entity->draw(shader);
    }
}

} // namespace tmig
