#pragma once

namespace tmig {

namespace gl {

class Texture {
public:
    Texture() = default;
    Texture(const std::string &path);

    void activate(const unsigned int unit) const;
    void bind() const;
    void unbind() const;
    void destroy() const;

private:
    unsigned int id = 0;
};

} // namespace gl

} // namespace tmig