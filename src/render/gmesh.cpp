#include "tmig/render/gmesh.hpp"

#include "glad/glad.h"

namespace tmig {

namespace render {

GMesh::GMesh(const Mesh &mesh)
    : Mesh{mesh}
{
    setup();
}

void GMesh::setup() {
    // Create and set VBO and EBO
    vao = gl::VAO::create();
    vao->bind();
    vbo = gl::VBO::create(vertices);
    vbo->bind();
    ebo = gl::EBO::create(indices);
    ebo->bind();

    // Set vertex attributes
    vao->vertexAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (const void *)(offsetof(Vertex, pos)));
    vao->vertexAttrib(*vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (const void *)(offsetof(Vertex, uv)));
    vao->vertexAttrib(*vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (const void *)(offsetof(Vertex, normal)));

    vao->unbind();
    vbo->unbind();
    ebo->unbind();
}

} // namespace render

} // namespace tmig