#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace tmig {

namespace gl {

class Shader {
public:
    // the program id
    unsigned int id = 0;

    Shader();

    // Constructor
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    // Use/activate shader
    void use() const;

    // Destroy shader
    void destroy() const;

    // Set uniform bool in shader
    void setBool(const std::string &name, bool value) const;

    // Set uniform int in shader
    void setInt(const std::string &name, int value) const;

    // Set uniform float in shader
    void setFloat(const std::string &name, float value) const;

    // Set uniform vec2 in shader
    void setVec2(const std::string &name, glm::vec2 &v) const;

    // Set uniform vec3 in shader
    void setVec3(const std::string &name, glm::vec3 v) const;

    // Set uniform vec4 in shader
    void setVec4(const std::string &name, glm::vec4 &v) const;

    // Set uniform mat4 in shader
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};

} // namespace gl

} // namespace tmig