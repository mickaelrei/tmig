#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"

#include "tmig/render/shader.hpp"
#include "tmig/util/debug.hpp"

namespace tmig::render {

std::shared_ptr<Shader> Shader::create(
    const std::string &vertexPath,
    const std::string &fragmentPath
) {
    return std::shared_ptr<Shader>{new Shader{vertexPath, fragmentPath}, Deleter{}};
}

Shader::Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
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
        throw std::runtime_error{"error reading shader source files"};
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile shaders
    unsigned int vertex, fragment;
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
    id = glCreateProgram(); glCheckError();
    util::debugPrint("Created shader: %d\n", id);

    glAttachShader(id, vertex); glCheckError();
    glAttachShader(id, fragment); glCheckError();
    glLinkProgram(id); glCheckError();
    // Print linking errors if any
    glGetProgramiv(id, GL_LINK_STATUS, &success); glCheckError();
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog); glCheckError();
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << "\n";
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex); glCheckError();
    glDeleteShader(fragment); glCheckError();
}

void Shader::use() const {
    glUseProgram(id); glCheckError();
}

void Shader::setBool(const std::string &name, bool value) const {
    use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value); glCheckError();
}

void Shader::setInt(const std::string &name, int value) const {
    use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), value); glCheckError();
}

void Shader::setFloat(const std::string &name, float value) const {
    use();
    glUniform1f(glGetUniformLocation(id, name.c_str()), value); glCheckError();
}

void Shader::setVec2(const std::string &name, const glm::vec2 &v) const {
    use();
    glUniform2f(glGetUniformLocation(id, name.c_str()), v.x, v.y); glCheckError();
}

void Shader::setVec3(const std::string &name, const glm::vec3 v) const {
    use();
    glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z); glCheckError();
}

void Shader::setVec4(const std::string &name, const glm::vec4 &v) const {
    use();
    glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w); glCheckError();
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    use();
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]); glCheckError();
}

void Shader::Deleter::operator() (Shader *shader) {
    glDeleteProgram(shader->id); glCheckError();
    util::debugPrint("Destroyed shader: %d\n", shader->id);
    shader->id = 0;
    delete shader;
}

} // namespace tmig::render