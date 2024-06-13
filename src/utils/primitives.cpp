#include "utils/primitives.hpp"
#include "utils/shaders.hpp"

Sphere::Sphere(const glm::vec3 &pos, const float radius)
    : _radius{radius}
{
    // TODO: Generate vertices and indices
    // const size_t resolution = 20;

    _position = pos;
    _scale = glm::vec3{radius};
    setup();
}

float Sphere::getRadius() const
{
    // Scale vector will always be the same on x, y and z
    return getScale().x;
}

void Sphere::setRadius(const float radius)
{
    setScale(glm::vec3{radius});
}

Cube::Cube(const glm::vec3 &pos, const glm::vec3 &size, const glm::vec4 &color)
    : _color{color}
{
    // Vertices for cube
    // std::vector<Vertex> vertices{
    //     Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},
    //     Vertex{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},
    //     Vertex{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},
    //     Vertex{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},

    //     Vertex{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
    //     Vertex{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
    //     Vertex{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
    //     Vertex{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
    // };

    // // Indices for cube
    // std::vector<unsigned int> indices{
    //     // back
    //     4, 6, 7,
    //     4, 5, 6,

    //     // right
    //     5, 2, 6,
    //     5, 1, 2,

    //     // left
    //     0, 7, 3,
    //     0, 4, 7,

    //     // front
    //     1, 3, 2,
    //     1, 0, 3,

    //     // top
    //     7, 2, 3,
    //     7, 6, 2,

    //     // bottom
    //     0, 5, 4,
    //     0, 1, 5,
    // };

    // Create mesh
    mesh = Mesh{
        std::vector<Vertex>{
            Vertex{glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},
            Vertex{glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},
            Vertex{glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},
            Vertex{glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec2{0.0f, 0.0f}, color},

            Vertex{glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
            Vertex{glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
            Vertex{glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
            Vertex{glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec2{0.0f, 0.0f}, color},
        },
        std::vector<unsigned int>{
            // back
            4, 6, 7,
            4, 5, 6,

            // right
            5, 2, 6,
            5, 1, 2,

            // left
            0, 7, 3,
            0, 4, 7,

            // front
            1, 3, 2,
            1, 0, 3,

            // top
            7, 2, 3,
            7, 6, 2,

            // bottom
            0, 5, 4,
            0, 1, 5,
        },
    };

    // Set shader
    shader = newDefaultEntityShader();
    
    // Update model matrix
    _position = pos;
    _scale = size;
    setup();
}

glm::vec4 Cube::getColor() const
{
    return _color;
}

void Cube::setColor(const glm::vec4 &color)
{
    // Change color in all vertices
    for (auto &v : mesh.vertices) {
        v.color = color;
    }

    _color = color;
}
