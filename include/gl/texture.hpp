#pragma once

#include <vector>

class Texture
{
public:
    Texture();
    Texture(const std::string &path);

    void activate(const unsigned int unit) const;
    void bind() const;
    void unbind() const;
    void destroy() const;

    unsigned int id = 0;
};