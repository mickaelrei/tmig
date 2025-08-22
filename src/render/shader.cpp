#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"

#include "tmig/render/shader.hpp"
#include "tmig/util/log.hpp"
#include "tmig/util/file.hpp"

namespace tmig::render {

ShaderProgram::~ShaderProgram() {
    if (_id == 0) return;

    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "Deleting shader: %u\n", _id
    );
    glDeleteProgram(_id); glCheckError();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : _id{other._id},
      _linked{other._linked},
      uniformLocationCache{std::move(other.uniformLocationCache)}
{
    other._id = 0;
    other._linked = false;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        if (_id != 0) {
            glDeleteProgram(_id);
        }

        _id = other._id;
        _linked = other._linked;
        uniformLocationCache = std::move(other.uniformLocationCache);

        other._id = 0;
        other._linked = false;
    }
    return *this;
}

bool ShaderProgram::compileFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    // Delete old program if any
    if (_id != 0) {
        util::logMessage(
            util::LogCategory::ENGINE, util::LogSeverity::INFO,
            "Deleting shader program: %u\n", _id
        );
        glDeleteProgram(_id);
        _id = 0;
        _linked = false;
    }

    // Create new program
    _id = glCreateProgram();
    if (_id == 0) {
        util::logMessage(
            util::LogCategory::OPENGL, util::LogSeverity::ERROR,
            "Failed to create shader program\n"
        );
        return false;
    }

    util::logMessage(
        util::LogCategory::OPENGL, util::LogSeverity::INFO,
        "Created shader: %u\n", _id
    );

    // Attempt to read files
    std::string vertexCode, fragmentCode;
    try {
        vertexCode   = util::readFileContent(vertexPath);
        fragmentCode = util::readFileContent(fragmentPath);
    } catch (const std::exception& e) {
        util::logMessage(util::LogCategory::ENGINE, util::LogSeverity::ERROR, "%s\n", e.what());
        return false;
    }

    const char* vCode = vertexCode.c_str();
    const char* fCode = fragmentCode.c_str();

    uint32_t vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Attempt to compile shaders
    glShaderSource(vertexShader, 1, &vCode, nullptr);
    glShaderSource(fragmentShader, 1, &fCode, nullptr);

    bool vOk = compileShaderStage(vertexShader,   "Vertex Shader");
    bool fOk = compileShaderStage(fragmentShader, "Fragment Shader");

    if (!(vOk && fOk)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Attach shaders to program and link
    glAttachShader(_id, vertexShader);
    glAttachShader(_id, fragmentShader);
    glLinkProgram(_id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Check link status
    int linkStatus;
    glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        char infoLog[1024];
        glGetProgramInfoLog(_id, sizeof(infoLog), nullptr, infoLog);
        util::logMessage(
            util::LogCategory::SHADER, util::LogSeverity::ERROR,
            "Program linking failed:\n%s\n", infoLog
        );
        return false;
    }

    _linked = true;
    util::logMessage(
        util::LogCategory::ENGINE, util::LogSeverity::INFO,
        "Shader program %u linked\n", _id
    );
    return true;
}

void ShaderProgram::use() const {
#ifdef DEBUG
    if (!_linked) {
        util::logMessage(
            util::LogCategory::OPENGL, util::LogSeverity::ERROR,
            "Attempted to use an unlinked shader program (id=%u)\n", _id
        );
        throw std::runtime_error{"[ShaderProgram::use] Attempt to use an unlinked shader program"};
    }
#endif

    glUseProgram(_id); glCheckError();
}

void ShaderProgram::setBool(const std::string& name, bool value) {
    if (!_linked) return;

    setInt(name, static_cast<int>(value));
}

void ShaderProgram::setInt(const std::string& name, int value) {
    if (!_linked) return;

    glProgramUniform1i(_id, getUniformLocation(name), value); glCheckError();
}

void ShaderProgram::setFloat(const std::string& name, float value) {
    if (!_linked) return;

    glProgramUniform1f(_id, getUniformLocation(name), value); glCheckError();
}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& v) {
    if (!_linked) return;

    glProgramUniform2f(_id, getUniformLocation(name), v.x, v.y); glCheckError();
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& v) {
    if (!_linked) return;

    glProgramUniform3f(_id, getUniformLocation(name), v.x, v.y, v.z); glCheckError();
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& v) {
    if (!_linked) return;

    glProgramUniform4f(_id, getUniformLocation(name), v.x, v.y, v.z, v.w); glCheckError();
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) {
    if (!_linked) return;

    glProgramUniformMatrix4fv(_id, getUniformLocation(name), 1, GL_FALSE, &mat[0][0]); glCheckError();
}

void ShaderProgram::setTexture(const std::string& name, const Texture2D& texture, uint32_t unit) {
    if (!_linked) return;

    texture.bind(unit);
    setInt(name, static_cast<int>(unit));
}

int ShaderProgram::getUniformLocation(const std::string& name) {
    if (!_linked) return 0;

    auto it = uniformLocationCache.find(name);
    if (it != uniformLocationCache.end()) {
        return it->second;
    }

    int location = glGetUniformLocation(_id, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

bool ShaderProgram::compileShaderStage(uint32_t shader, const char* typeName) {
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        util::logMessage(util::LogCategory::SHADER, util::LogSeverity::ERROR,
                         "%s compilation failed:\n%s\n", typeName, infoLog);
        return false;
    }

    util::logMessage(util::LogCategory::SHADER, util::LogSeverity::INFO,
                     "%s compiled\n", typeName);
    return true;
}

} // namespace tmig::render