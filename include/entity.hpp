#pragma once

#include <vector>

#include "gl/vao.hpp"
#include "gl/vbo.hpp"
#include "gl/ebo.hpp"
#include "gl/shader.hpp"
#include "gl/texture.hpp"
#include "mesh.hpp"

class Entity : public Mesh
{
public:
    Entity();
    Entity(
        const Mesh &mesh,
        const Shader &shader,
        const std::vector<Texture> &textures = {},
        const glm::vec3 &pos = glm::vec3{0.0f},
        const glm::mat4 &rotation = glm::mat4{1.0f},
        const glm::vec3 &scale = glm::vec3{1.0f});

    void destroy();
    
    void setViewMatrix(const glm::mat4 &view);
    void setProjectionMatrix(const glm::mat4 &projection);

    glm::vec3 getPosition() const;
    glm::mat4 getRotation() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::mat4 &rotation);
    void setScale(const glm::vec3 &scale);

    void translate(const glm::vec3 &offset);
    void rotate(const glm::mat4 &rotation);

    void update(float dt);
    void draw(const glm::mat4 &mat = glm::mat4{1.0f});

protected:
    Mesh mesh;

    glm::vec3 _position;
    glm::mat4 _rotation;
    glm::vec3 _scale;
    glm::mat4 _modelMatrix;

    VAO vao;
    VBO vbo;
    EBO ebo;
    Shader shader;
    std::vector<Texture> textures;

    void updateModelMatrix();
    void setup();

private:
    bool _setupCalled = false;
};