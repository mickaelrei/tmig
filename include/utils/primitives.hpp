#pragma once

#include "entity.hpp"

class Sphere : public Entity
{
public:
    Sphere(const glm::vec3 &pos, const float radius);

    float getRadius() const;
    void setRadius(const float radius);

private:
    glm::vec3 _pos;
    float _radius;
};


class Cube : public Entity
{
public:
    Cube(const glm::vec3 &pos = glm::vec3{0.0f}, const glm::vec3 &size = glm::vec3{1.0f}, const glm::vec4 &color = glm::vec4{1.0f});

    glm::vec4 getColor() const;
    void setColor(const glm::vec4 &color);

private:
    glm::vec4 _color;
};