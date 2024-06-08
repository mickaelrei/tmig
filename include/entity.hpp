#pragma once

#include <vector>

#include "mesh.hpp"

class Entity : public Mesh
{
public:
    Entity();
    Entity(const Mesh &mesh);
    ~Entity();

    glm::vec3 getPosition() const;
    glm::mat4 getRotation() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::mat4 &rotation);
    void setScale(const glm::vec3 &scale);

    void translate(const glm::vec3 &offset);
    void rotate(const glm::mat4 &rotation);

    void update(float dt) override;
    void draw(const glm::mat4 &mat = glm::mat4{1.0f}) override;

private:
    glm::vec3 _position;
    glm::mat4 _rotation;
    glm::vec3 _scale;

    glm::mat4 _modelMatrix;
};