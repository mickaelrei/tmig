#pragma once

#include <entity.hpp>

class Sphere : public Entity
{
public:
    Sphere(const glm::vec3 &pos, const float radius);

    float getRadius() const;
    void setRadius(const float radius);

private:
    float _radius;
};