#pragma once

#include <vector>

#include "tmig/render/gl/shader.hpp"
#include "tmig/render/gl/texture.hpp"
#include "tmig/render/gmesh.hpp"

namespace tmig {

namespace render {

/// @brief Class that represents a graphic entity in 3D space
class Entity
{
public:
    /// @brief Default constructor
    Entity() = default;

    /// @brief Constructor with parameters
    /// @param gmesh gmesh to assign to entity
    /// @param textures optional list of textures
    Entity(
        const GMesh &gmesh,
        const std::vector<std::shared_ptr<gl::Texture>> &textures = {}
    );

    /// @brief Get current position
    /// @return vec3 for entity position
    virtual glm::vec3 getPosition() const;

    /// @brief Get current rotation matrix
    /// @return mat4 for entity rotation
    virtual glm::mat4 getRotation() const;

    /// @brief Get current scale
    /// @return vec3 for entity scale
    virtual glm::vec3 getScale() const;

    /// @brief Get current color
    /// @return vec4 for entity color
    virtual glm::vec4 getColor() const;

    /// @brief Set entity position
    /// @param position new position
    virtual void setPosition(const glm::vec3 &position);

    /// @brief Set entity rotation
    /// @param rotation new rotation
    virtual void setRotation(const glm::mat4 &rotation);

    /// @brief Set entity scale
    /// @param scale new scale
    virtual void setScale(const glm::vec3 &scale);

    /// @brief Set entity color
    /// @param color new color
    virtual void setColor(const glm::vec4 &color);

    /// @brief Apply position offset on entity
    /// @param offset position to offset
    virtual void translate(const glm::vec3 &offset);

    /// @brief Apply rotation on entity
    /// @param rotation rotation to be applied
    virtual void rotate(const glm::mat4 &rotation);

    /// @brief Update entity (this is useful for inherited classes with special behavior)
    /// @param dt delta time
    virtual void update(float dt);

    /// @brief Draw entity
    /// @param shader scene shader
    virtual void draw(const gl::Shader &shader) const;

    /// @brief Gets entity mesh
    /// @return entity mesh
    Mesh getMesh() const;

    /// @brief Returns read-only pointer to model matrix (used on colliders)
    /// @return pointer to model matrix
    const glm::mat4 *getModelMatrixPointer() const;

    /// @brief Returns read-only pointer to position vector (used on colliders)
    /// @return pointer to position vector
    const glm::vec3 *getPositionPointer() const;

    /// @brief Returns read-only pointer to scale vector (used on colliders)
    /// @return pointer to scale vector
    const glm::vec3 *getScalePointer() const;

protected:
    /// @brief Entity graphic mesh
    GMesh gmesh;

    /// @brief Current position
    glm::vec3 _position = glm::vec3{0.0f};

    /// @brief Current rotation
    glm::mat4 _rotation = glm::mat4{1.0f};

    /// @brief Current scale
    glm::vec3 _scale = glm::vec3{1.0f};

    /// @brief Current color
    glm::vec4 _color = glm::vec4{1.0f};

    /// @brief Model matrix for caching
    glm::mat4 _modelMatrix = glm::mat4{1.0f};

    /// @brief List of textures
    std::vector<std::shared_ptr<gl::Texture>> textures;

    /// @brief Updates cached model matrix
    void updateModelMatrix();
};

/// @brief Max number of textures an entity can have
const size_t maxTextures = 4;

} // namespace render

} // namespace tmig