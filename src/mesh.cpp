#include "mesh.hpp"

Mesh::Mesh()
{
}

Mesh::Mesh(
    const std::vector<Vertex> &vertices,
    const std::vector<unsigned int> &indices,
    const Shader &shader)
    : vertices{vertices},
      indices{indices},
      shader{shader}
{
    setup();
}

Mesh::~Mesh()
{
    vbo.destroy();
    ebo.destroy();
    vao.destroy();
    shader.destroy();
}

void Mesh::setup()
{
    // Set VAO, VBO and EBO

    // Set matrices in shader

    // etc
}

void Mesh::update(float dt)
{
}

void Mesh::draw(const glm::mat4 &mat = glm::mat4{1.0f})
{
    // Update mat

    // Bind VAO

    // Draw elements
}
