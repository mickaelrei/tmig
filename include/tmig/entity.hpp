#pragma once

#include <vector>

#include "tmig/gl/vao.hpp"
#include "tmig/gl/vbo.hpp"
#include "tmig/gl/ebo.hpp"
#include "tmig/gl/shader.hpp"
#include "tmig/gl/texture.hpp"
#include "tmig/mesh.hpp"

namespace tmig {

class Entity : public Mesh
{
public:
    Entity();
    Entity(
        const Mesh &mesh,
        const gl::Shader &shader,
        const std::vector<gl::Texture> &textures = {},
        const glm::vec3 &pos = glm::vec3{0.0f},
        const glm::mat4 &rotation = glm::mat4{1.0f},
        const glm::vec3 &scale = glm::vec3{1.0f},
        const glm::vec4 &color = glm::vec4{1.0f}
    );

    void destroy();

    void setViewMatrix(const glm::mat4 &view) const;
    void setProjectionMatrix(const glm::mat4 &projection) const;

    glm::vec3 getPosition() const;
    glm::mat4 getRotation() const;
    glm::vec3 getScale() const;
    glm::vec4 getColor() const;

    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::mat4 &rotation);
    void setScale(const glm::vec3 &scale);
    void setColor(const glm::vec4 &color);

    void translate(const glm::vec3 &offset);
    void rotate(const glm::mat4 &rotation);

    virtual void update(float dt);
    virtual void draw(const glm::mat4 &mat = glm::mat4{1.0f}) const;

protected:
    Mesh mesh;

    glm::vec3 _position;
    glm::mat4 _rotation;
    glm::vec3 _scale;
    glm::mat4 _modelMatrix;
    glm::vec4 _color;

    gl::VAO vao;
    gl::VBO vbo;
    gl::EBO ebo;
    gl::Shader shader;
    std::vector<gl::Texture> textures;

    void updateModelMatrix();
    void setup();

private:
    bool _setupCalled = false;
};

} // namespace tmig