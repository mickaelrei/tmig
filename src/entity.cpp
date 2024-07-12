#include <glm/gtc/matrix_transform.hpp>

#include "glad/glad.h"

#include "tmig/entity.hpp"

namespace tmig {

Entity::Entity(const GMesh &gmesh, const std::vector<gl::Texture> &textures)
    : gmesh{gmesh},
      textures{textures} {}

void Entity::destroy() {
}

glm::vec3 Entity::getPosition() const {
    return _position;
}

glm::mat4 Entity::getRotation() const {
    return _rotation;
}

glm::vec3 Entity::getScale() const {
    return _scale;
}

glm::vec4 Entity::getColor() const {
    return _color;
}

void Entity::setPosition(const glm::vec3 &position) {
    _position = position;
    updateModelMatrix();
}

void Entity::setRotation(const glm::mat4 &rotation) {
    _rotation = rotation;
    updateModelMatrix();
}

void Entity::setScale(const glm::vec3 &scale) {
    _scale = scale;
    updateModelMatrix();
}

void Entity::setColor(const glm::vec4 &color) {
    _color = color;
}

void Entity::translate(const glm::vec3 &offset) {
    _position += offset;
    updateModelMatrix();
}

void Entity::rotate(const glm::mat4 &rotation) {
    _rotation *= rotation;
    updateModelMatrix();
}

void Entity::updateModelMatrix() {
    _modelMatrix = glm::mat4{1.0f};
    _modelMatrix = glm::translate(_modelMatrix, _position);
    _modelMatrix *= _rotation;
    _modelMatrix = glm::scale(_modelMatrix, _scale);
}

void Entity::update(float dt) {
    (void)dt;
}

void Entity::draw(const gl::Shader &shader) const {
    // Bind shader and VAO
    shader.use();
    shader.setMat4("model", _modelMatrix);
    shader.setVec4("meshColor", _color);
    gmesh.vao.bind();

    // Set textures
    int numTextures = (int)std::min(maxTextures, textures.size());
    for (int i = 0; i < numTextures; ++i) {
        textures[i].activate(i);
        textures[i].bind();
        shader.setInt("textures[" + std::to_string(i) + "]", i);
    }
    shader.setInt("numTextures", numTextures);

    // Draw
    glDrawElements(GL_TRIANGLES, gmesh.indices.size(), GL_UNSIGNED_INT, 0);
    gmesh.vao.unbind();
}

} // namespace tmig