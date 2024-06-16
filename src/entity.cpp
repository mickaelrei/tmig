#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include <glad/glad.h>

#include "entity.hpp"

namespace tmig {

Entity::Entity() {}

Entity::Entity(const Mesh &mesh,
    const gl::Shader &shader,
    const std::vector<gl::Texture> &textures,
    const glm::vec3 &pos,
    const glm::mat4 &rotation,
    const glm::vec3 &scale,
    const glm::vec4 &color
)
    : mesh{mesh},
    _position{pos},
    _rotation{rotation},
    _scale{scale},
    _color{color},
    shader{shader},
    textures{textures}
{
    setup();
}

void Entity::destroy()
{
    ebo.destroy();
    vbo.destroy();
    vao.destroy();
}

void Entity::setViewMatrix(const glm::mat4 &view)
{
    shader.setMat4("view", view);
}

void Entity::setProjectionMatrix(const glm::mat4 &projection)
{
    shader.setMat4("projection", projection);
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

glm::vec4 Entity::getColor() const
{
    return _color;
}

void Entity::setPosition(const glm::vec3 &position)
{
    _position = position;
    updateModelMatrix();
}

void Entity::setRotation(const glm::mat4 &rotation)
{
    _rotation = rotation;
    updateModelMatrix();
}

void Entity::setScale(const glm::vec3 &scale)
{
    _scale = scale;
    updateModelMatrix();
}

void Entity::setColor(const glm::vec4 &color)
{
    _color = color;
}


void Entity::translate(const glm::vec3 &offset)
{
    _position += offset;
    updateModelMatrix();
}

void Entity::rotate(const glm::mat4 &rotation)
{
    _rotation *= rotation;
    updateModelMatrix();
}

void Entity::updateModelMatrix()
{
    _modelMatrix = glm::mat4{1.0f};
    _modelMatrix = glm::translate(_modelMatrix, _position);
    _modelMatrix *= _rotation;
    _modelMatrix = glm::scale(_modelMatrix, _scale);

    shader.use();
    shader.setMat4("model", _modelMatrix);
}

void Entity::update(float dt)
{
    (void)dt;
}

void Entity::draw(const glm::mat4 &mat)
{
    // Bind shader and VAO
    shader.use();
    shader.setMat4("model", mat * _modelMatrix);
    shader.setVec4("color", _color);
    vao.bind();

    // Set textures
    for (unsigned int i = 0; i < textures.size(); ++i) {
        textures[i].activate(i);
        textures[i].bind();
        shader.setInt("texture" + std::to_string(i), i);
    }

    // Draw
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();
}

void Entity::setup()
{
    // Setup is supposed to be called only once
    if (_setupCalled) return;
    _setupCalled = true;

    updateModelMatrix();

    // Create and set VBO and EBO
    vao.bind();
    vbo = gl::VBO{mesh.vertices};
    vbo.bind();
    ebo = gl::EBO{mesh.indices};
    ebo.bind();

    // Set vertex attributes
    vao.vertexAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (const void *)0);
    vao.vertexAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (const void *)(sizeof(glm::vec3)));

    vao.unbind();
    vbo.unbind();
    ebo.unbind();
}

} // namespace tmig