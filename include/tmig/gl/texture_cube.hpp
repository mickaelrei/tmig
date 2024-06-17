#pragma once

#include <string>

namespace tmig {

namespace gl {

class TextureCube {
public:
    TextureCube() = default;
    TextureCube(
        const std::string &rightPath,
        const std::string &leftPath,
        const std::string &topPath,
        const std::string &bottomPath,
        const std::string &frontPath,
        const std::string &backPath
    );

    void bind() const;
    void unbind() const;
    void destroy() const;

    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig