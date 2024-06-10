#include <glad/glad.h>

#include "entity.hpp"

Entity::Entity() {}

Entity::Entity(const Mesh &mesh,
               const Shader &shader,
               const std::vector<Texture> &textures,
               const glm::vec3 &pos,
               const glm::mat4 &rotation,
               const glm::vec3 &scale)
    : mesh{mesh},
      _position{pos},
      _rotation{rotation},
      _scale{scale},
      shader{shader},
      textures{textures}
{
    updateModelMatrix();

    // Create and set VBO and EBO
    vao.bind();
    vbo = VBO{mesh.vertices};
    vbo.bind();
    ebo = EBO{mesh.indices};
    ebo.bind();

    // Set vertex attributes
    vao.vertexAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (const void *)0);
    vao.vertexAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (const void *)(3 * sizeof(float)));

    vao.unbind();
    vbo.unbind();
    ebo.unbind();
}

void Entity::destroy()
{
    ebo.destroy();
    vbo.destroy();
    vao.destroy();
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
    (void)mat;

    // Bind shader and VAO
    shader.use();
    vao.bind();

    // Set textures
    for (unsigned int i = 0; i < textures.size(); ++i) {
        textures[i].bind();
        textures[i].activate(i);
        shader.setInt("texture" + std::to_string(i), i);
    }

    // Draw
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();
}