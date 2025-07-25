#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "tmig/core/non_copyable.hpp"

namespace tmig::render {

/// @brief OpenGL shader wrapper class
/// @note - This is a non-copyable class, meaning you cannot create a copy of it.
class Shader : protected core::NonCopyable {
public:
    /// @brief Constructor with file paths
    /// @param vertexPath Path to vertex shader file
    /// @param fragmentPath Path to frament shader file
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /// @brief Destructor
    virtual ~Shader();

    /// @brief Move constructor
    Shader(Shader&& other) noexcept;

    /// @brief Move assignment
    Shader& operator=(Shader&& other) noexcept;

    /// @brief Use/activate shader
    void use() const;

    /// @brief Set uniform bool in shader
    void setBool(const std::string& name, bool value) const;

    /// @brief Set uniform int in shader
    void setInt(const std::string& name, int value) const;

    /// @brief Set uniform float in shader
    void setFloat(const std::string& name, float value) const;

    /// @brief Set uniform vec2 in shader
    void setVec2(const std::string& name, const glm::vec2& v) const;

    /// @brief Set uniform vec3 in shader
    void setVec3(const std::string& name, const glm::vec3& v) const;

    /// @brief Set uniform vec4 in shader
    void setVec4(const std::string& name, const glm::vec4& v) const;

    /// @brief Set uniform mat4 in shader
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    /// @brief OpenGL identifier
    unsigned int _id = 0;
};

} // namespace tmig::render