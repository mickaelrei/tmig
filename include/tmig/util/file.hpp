#pragma once

#include <string>

namespace tmig::util {

/// @brief Reads the content of a file and returns as a string
/// @note Throws an `std::runtime_error` if reading fails
std::string readFileContent(const std::string& filePath);

} // namespace tmig::util