#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"

#include "tmig/render/shader.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Open files
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);

        // Read file's buffer contents into streams
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error{"[Shader::Shader] error reading shader source files"};
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    uint32_t vertex, fragment;
    int success;
    char infoLog[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER); glCheckError();
    glShaderSource(vertex, 1, &vShaderCode, NULL); glCheckError();
    glCompileShader(vertex); glCheckError();
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success); glCheckError();
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog); glCheckError();
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << "\n";
    };

    // Similiar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER); glCheckError();
    glShaderSource(fragment, 1, &fShaderCode, NULL); glCheckError();
    glCompileShader(fragment); glCheckError();
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success); glCheckError();
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog); glCheckError();
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << "\n";
    };

    // Shader Program
    _id = glCreateProgram(); glCheckError();
    util::debugPrint("Created shader: %u\n", _id);

    glAttachShader(_id, vertex); glCheckError();
    glAttachShader(_id, fragment); glCheckError();
    glLinkProgram(_id); glCheckError();
    // Print linking errors if any
    glGetProgramiv(_id, GL_LINK_STATUS, &success); glCheckError();
    if (!success) {
        glGetProgramInfoLog(_id, 512, NULL, infoLog); glCheckError();
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << "\n";
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex); glCheckError();
    glDeleteShader(fragment); glCheckError();
}

Shader::~Shader() {
    util::debugPrint("Deleting shader: %u\n", _id);
    glDeleteProgram(_id); glCheckError();
}

Shader::Shader(Shader&& other) noexcept
    : _id{other._id}
{
    other._id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        glDeleteProgram(_id);

        _id = other._id;
        other._id = 0;
    }
    return *this;
}

void Shader::use() const {
    glUseProgram(_id); glCheckError();
}

void Shader::setBool(const std::string& name, bool value) const {
    use();
    glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value); glCheckError();
}

void Shader::setInt(const std::string& name, int value) const {
    use();
    glUniform1i(glGetUniformLocation(_id, name.c_str()), value); glCheckError();
}

void Shader::setFloat(const std::string& name, float value) const {
    use();
    glUniform1f(glGetUniformLocation(_id, name.c_str()), value); glCheckError();
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) const {
    use();
    glUniform2f(glGetUniformLocation(_id, name.c_str()), v.x, v.y); glCheckError();
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    use();
    glUniform3f(glGetUniformLocation(_id, name.c_str()), v.x, v.y, v.z); glCheckError();
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const {
    use();
    glUniform4f(glGetUniformLocation(_id, name.c_str()), v.x, v.y, v.z, v.w); glCheckError();
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    use();
    glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]); glCheckError();
}

} // namespace tmig::render