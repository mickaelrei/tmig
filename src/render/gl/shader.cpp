#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"

#include "tmig/utils.hpp"
#include "tmig/render/gl/shader.hpp"

namespace tmig {

namespace render {

namespace gl {

 std::shared_ptr<Shader> Shader::create(
    const std::string &vertexPath,
    const std::string &fragmentPath
) {
    return std::shared_ptr<Shader>{new Shader{vertexPath, fragmentPath}, Deleter{}};
}

Shader::Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
{
    // Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
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
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
            << infoLog << "\n";
    };

    // Similiar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
            << infoLog << "\n";
    };

    // Shader Program
    _id = glCreateProgram();
    debug_print("Created shader: %d\n", _id);

    glAttachShader(_id, vertex);
    glAttachShader(_id, fragment);
    glLinkProgram(_id);
    // Print linking errors if any
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog << "\n";
    }

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const
{
    glUseProgram(_id);
}

void Shader::destroy()
{
    debug_print("Destroyed shader: %d\n", _id);
    glDeleteProgram(_id);
}

void Shader::setBool(const std::string &name, const bool value) const
{
    use();
    glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, const int value) const
{
    use();
    glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const float value) const
{
    use();
    glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &v) const
{
    use();
    glUniform2f(glGetUniformLocation(_id, name.c_str()), v.x, v.y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 v) const
{
    use();
    glUniform3f(glGetUniformLocation(_id, name.c_str()), v.x, v.y, v.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &v) const
{
    use();
    glUniform4f(glGetUniformLocation(_id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    use();
    glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

} // namespace gl

} // namespace render

} // namespace tmig