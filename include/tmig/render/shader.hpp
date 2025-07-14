#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace tmig::render {

/// @brief OpenGL shader wrapper class
class Shader {
public:
    /// @brief Creates a new shader
    /// @param vertexPath Path to vertex shader file
    /// @param fragmentPath Path to frament shader file
    /// @return Shared pointer to new shader
    static std::shared_ptr<Shader> create(
        const std::string &vertexPath,
        const std::string &fragmentPath
    );

    /// @brief Use/activate shader
    void use() const;

    /// @brief Set uniform bool in shader
    void setBool(const std::string &name, bool value) const;

    /// @brief Set uniform int in shader
    void setInt(const std::string &name, int value) const;

    /// @brief Set uniform float in shader
    void setFloat(const std::string &name, float value) const;

    /// @brief Set uniform vec2 in shader
    void setVec2(const std::string &name, const glm::vec2 &v) const;

    /// @brief Set uniform vec3 in shader
    void setVec3(const std::string &name, const glm::vec3 v) const;

    /// @brief Set uniform vec4 in shader
    void setVec4(const std::string &name, const glm::vec4 &v) const;

    /// @brief Set uniform mat4 in shader
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    /// @brief Constructor with file paths
    /// @param vertexPath Path to vertex shader file
    /// @param fragmentPath Path to frament shader file
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    /// @brief Deleter struct
    struct Deleter {
        void operator()(Shader *shader);
    };

    /// @brief OpenGL identifier
    unsigned int id = 0;
};

} // namespace tmig::render