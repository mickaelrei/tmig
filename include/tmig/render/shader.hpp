#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>

#include <glm/glm.hpp>

#include "tmig/render/texture2D.hpp"
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
    void setBool(const std::string& name, bool value);

    /// @brief Set uniform int in shader
    void setInt(const std::string& name, int value);

    /// @brief Set uniform float in shader
    void setFloat(const std::string& name, float value);

    /// @brief Set uniform vec2 in shader
    void setVec2(const std::string& name, const glm::vec2& v);

    /// @brief Set uniform vec3 in shader
    void setVec3(const std::string& name, const glm::vec3& v);

    /// @brief Set uniform vec4 in shader
    void setVec4(const std::string& name, const glm::vec4& v);

    /// @brief Set uniform mat4 in shader
    void setMat4(const std::string& name, const glm::mat4& mat);

    /// @brief Helper for setting a uniform texture in shader. Internally just calls `setInt` with `unit`
    void setTexture(const std::string& name, const Texture2D& texture, uint32_t unit);

private:
    /// @brief OpenGL identifier
    uint32_t _id = 0;

    /// @brief Uniform location cache
    std::unordered_map<std::string, int> uniformLocationCache;

    /// @brief Get cached uniform location, or query and store if not cached yet
    int getUniformLocation(const std::string& name);
};

} // namespace tmig::render