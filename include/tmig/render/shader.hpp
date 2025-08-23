#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

#include <glm/glm.hpp>

#include "tmig/render/texture2D.hpp"
#include "tmig/core/non_copyable.hpp"

namespace tmig::render {

/// @brief OpenGL shader program wrapper class
/// @note - This is a non-copyable class, meaning you cannot create a copy of it.
class ShaderProgram : protected core::NonCopyable {
public:
    /// @brief Constructor
    /// @note Does nothing on its own; you need to call `compileFromFiles` to make the program valid
    ShaderProgram() = default;

    /// @brief Destructor
    virtual ~ShaderProgram();

    /// @brief Move constructor
    ShaderProgram(ShaderProgram&& other) noexcept;

    /// @brief Move assignment
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    /// @brief Attempts to compile from the given vertex and fragment shader files
    /// @return Whether compilation and linking succeeded
    bool compileFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /// @brief Use/activate shader
    /// @note Will throw an `std::runtime_error` if not valid. Check with `isValid()`
    void use() const;

    /// @brief Returns shader id
    uint32_t id() const { return _id; }

    /// @brief Whether this shader is valid for usage; a successful call to `compileFromFiles` assures that
    bool isValid() const { return _linked; }

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

    /// @brief Whether shader program is linked
    bool _linked = false;

    /// @brief Uniform location cache
    std::unordered_map<std::string, int> uniformLocationCache;

    /// @brief Get cached uniform location, or query and store if not cached yet
    int getUniformLocation(const std::string& name);

    /// @brief Compile a specified shader stage
    /// @return Whether compilation succeeded
    bool compileShaderStage(uint32_t shader, const char* typeName);
};

} // namespace tmig::render