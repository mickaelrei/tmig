#include "tmig/util/resources.hpp"

#ifndef PROJECT_ROOT_FOLDER
#define PROJECT_ROOT_FOLDER ""
#endif

namespace tmig::util {

std::string getResourcePath(const std::string& path) {
    return std::string{PROJECT_ROOT_FOLDER} + "/resources/" + path;
}

} // namespace tmig::util