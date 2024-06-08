#include "entity.hpp"

Entity::Entity() {}

Entity::Entity(
    const std::vector<Vertex> &vertices,
    const std::vector<unsigned int> &indices,
    const Shader &shader)
    : vertices{vertices},
      indices{indices},
      shader{shader} {}

Entity::~Entity()
{
    shader.destroy();
}

glm::vec3 Entity::getPosition() const
{
    return _position;
}

glm::mat4 Entity::getRotation() const
{
    return _rotation;
}

glm::vec3 Entity::getScale() const
{
    return _scale;
}

void Entity::setPosition(const glm::vec3 &position)
{
    _position = position;
}

void Entity::setRotation(const glm::mat4 &rotation)
{
    _rotation = rotation;
}

void Entity::setScale(const glm::vec3 &scale)
{
    _scale = scale;
}

void Entity::translate(const glm::vec3 &offset)
{
    _position += offset;
}

void Entity::rotate(const glm::mat4 &rotation)
{
    _rotation *= rotation;
}
