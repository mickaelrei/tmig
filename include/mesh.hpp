#pragma once

#include <vector>

#include "vao.hpp"
#include "vbo.hpp"
#include "ebo.hpp"
#include "shader.hpp"
#include "vertex.hpp"

class Mesh
{
public:
    Mesh();
    Mesh(
        const std::vector<Vertex> &vertices,
        const std::vector<unsigned int> &indices,
        const Shader &shader);
    ~Mesh();

protected:
    virtual void setup();
    virtual void update(float dt);
    virtual void draw(const glm::mat4 &mat = glm::mat4{1.0f});

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    VAO vao;
    VBO vbo;
    EBO ebo;

    Shader shader;
};