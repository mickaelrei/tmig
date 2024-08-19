#include <glm/gtc/matrix_transform.hpp>

#include "tmig/render/utils/shaders.hpp"
#include "tmig/render/scene.hpp"

namespace tmig {

namespace render {

void Scene::addEntity(const std::shared_ptr<Entity> &entity)
{
    entities.push_back(entity);
}

void Scene::addTransparentEntity(const std::shared_ptr<Entity> &entity) {
    transparentEntities.push_back(entity);
}


void Scene::addLight(const std::shared_ptr<Light> &light)
{
    lights.push_back(light);
}

void Scene::setShader(const std::shared_ptr<gl::Shader> &shader) {
    Scene::shader = shader;
}

void Scene::setProjection(const glm::ivec2 &viewportSize) {
    auto projection = glm::perspective(
        glm::radians(camera.fov),
        (float)viewportSize.x / (float)viewportSize.y,
        camera.minDist, camera.maxDist
    );

    utils::skyboxShader()->setMat4("projection", projection);
    if (shader != nullptr) {
        shader->setMat4("projection", projection);
    }
}


void Scene::update(float dt) {
    (void)dt;

    // Only update lights if shader is set
    if (shader == nullptr) return;

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

        light->bind(*shader, prefix);
    }

    shader->setInt("numPointLights", numPointLights);
    shader->setInt("numDirectionalLights", numDirectionalLights);
    shader->setInt("numSpotLights", numSpotLights);

    // TODO: This could be better encapsulated as a config
    shader->setFloat("ambientStrength", 0.2f);
    shader->setFloat("specularStrength", 0.5f);
}

void Scene::render() const {
    auto view = camera.getViewMatrix();

    // Render skybox first
    if (renderSkybox) {
        auto skyboxShader = utils::skyboxShader();
        skyboxShader->setMat4("view", glm::mat4{glm::mat3{view}});
        skybox.draw(*skyboxShader);
    }

    // Draw all entities
    if (shader != nullptr) {
        shader->setMat4("view", view);

        // Draw opaque entities first
        for (const auto &entity : entities) {
            entity->draw(*shader);
        }

        // Then draw transparent entities with no depth writing
        glDepthMask(GL_FALSE);
        for (const auto &entity : transparentEntities) {
            entity->draw(*shader);
        }
        glDepthMask(GL_TRUE);
    }
}

} // namespace render

} // namespace tmig
