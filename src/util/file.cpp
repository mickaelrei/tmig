#include <fstream>
#include <sstream>
#include <stdexcept>

#include "tmig/util/file.hpp"

namespace tmig::util {

std::string readFileContent(const std::string& filePath) {
    std::ifstream file{filePath};
    if (!file) {
        throw std::runtime_error{"[util::readFileContent] Failed to open file: " + filePath};
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace tmig::util
