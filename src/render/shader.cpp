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
        if (_id != 0) {
            glDeleteProgram(_id);
        }

        _id = other._id;
        other._id = 0;
    }
    return *this;
}

void Shader::use() const {
    glUseProgram(_id); glCheckError();
}

void Shader::setBool(const std::string& name, bool value) {
    setInt(name, static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) {
    glProgramUniform1i(_id, getUniformLocation(name), value); glCheckError();
}

void Shader::setFloat(const std::string& name, float value) {
    glProgramUniform1f(_id, getUniformLocation(name), value); glCheckError();
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) {
    glProgramUniform2f(_id, getUniformLocation(name), v.x, v.y); glCheckError();
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) {
    glProgramUniform3f(_id, getUniformLocation(name), v.x, v.y, v.z); glCheckError();
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) {
    glProgramUniform4f(_id, getUniformLocation(name), v.x, v.y, v.z, v.w); glCheckError();
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glProgramUniformMatrix4fv(_id, getUniformLocation(name), 1, GL_FALSE, &mat[0][0]); glCheckError();
}

void Shader::setTexture(const std::string& name, const Texture2D& texture, uint32_t unit) {
    texture.bind(unit);
    setInt(name, static_cast<int>(unit));
}

int Shader::getUniformLocation(const std::string& name) {
    auto it = uniformLocationCache.find(name);
    if (it != uniformLocationCache.end()) {
        return it->second;
    }

    int location = glGetUniformLocation(_id, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

} // namespace tmig::render