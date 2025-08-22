#pragma once

#include <string>

namespace tmig::util {

/// @brief Returns the actual path for loading a resource at the given path
/// @return Path for the resource with necessary prefixes
/// @note Resources should always be placed under resources/
std::string getResourcePath(const std::string& path);

} // namespace tmig::util