#pragma once

#include <cstddef>
#include <vector>

namespace tmig {

namespace gl {

class EBO {
public:
    EBO();
    EBO(const std::vector<unsigned int> &indices);

    void bufferData(size_t size, const void *data) const;

    void bind() const;
    void unbind() const;
    void destroy() const;

    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig