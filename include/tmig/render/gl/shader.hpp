#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "tmig/render/gl/gl_object.hpp"

namespace tmig {

namespace render {

namespace gl {

/// @brief OpenGL shader wrapper class
class Shader : public GLObject {
public:
    /// @brief Default constructor (does nothing)
    Shader() = default;

    /// @brief Creates a new shader
    /// @param vertexPath path to vertex shader file
    /// @param fragmentPath path to frament shader file
    /// @return Shared pointer to new shader
    static std::shared_ptr<Shader> create(
        const std::string &vertexPath,
        const std::string &fragmentPath
    );

    /// @brief Use/activate shader
    void use() const;

    /// @brief Destroy shader
    void destroy() override;

    /// @brief Set uniform bool in shader
    /// @param name uniform name
    /// @param value uniform value
    void setBool(const std::string &name, const bool value) const;

    /// @brief Set uniform int in shader
    /// @param name uniform name
    /// @param value uniform value
    void setInt(const std::string &name, const int value) const;

    /// @brief Set uniform float in shader
    /// @param name uniform name
    /// @param value uniform value
    void setFloat(const std::string &name, const float value) const;

    /// @brief Set uniform vec2 in shader
    /// @param name uniform name
    /// @param v uniform value
    void setVec2(const std::string &name, const glm::vec2 &v) const;

    /// @brief Set uniform vec3 in shader
    /// @param name uniform name
    /// @param v uniform value
    void setVec3(const std::string &name, const glm::vec3 v) const;

    /// @brief Set uniform vec4 in shader
    /// @param name uniform name
    /// @param v uniform value
    void setVec4(const std::string &name, const glm::vec4 &v) const;

    /// @brief Set uniform mat4 in shader
    /// @param name uniform name
    /// @param mat uniform value
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    /// @brief Constructor with file paths
    /// @param vertexPath path to vertex shader file
    /// @param fragmentPath path to frament shader file
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
};

} // namespace gl

} // namespace render

} // namespace tmig