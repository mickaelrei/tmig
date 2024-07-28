#include <stdexcept>

#include "glad/glad.h"
#include "stb/stb_image.h"

#include "tmig/utils.hpp"
#include "tmig/gl/texture.hpp"

namespace tmig {

namespace gl {

std::shared_ptr<Texture> Texture::create(const std::string &path) {
    return std::shared_ptr<Texture>{new Texture{path}, Deleter{}};
}

Texture::Texture(const std::string &path)
{
    // Try loading image
    // TODO: Move this to a separate loadImageFromFile() util function
    stbi_set_flip_vertically_on_load(true);
    int width, height, numChannels;
    unsigned char *imageData = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
    if (imageData == NULL)
    {
        throw std::invalid_argument{"Could not load image: " + path};
    }

    // Get image format
    int format;
    switch (numChannels)
    {
    case 4:
        format = GL_RGBA;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 1:
        format = GL_RED;
        break;
    default:
        throw std::invalid_argument{"Could not recognize image format: " + path};
    }

    // Everything ok, generate texture
    glGenTextures(1, &_id);
    debug_print("Created texture: %d\n", _id);

    // Bind texture and set
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    // TODO: These could be given as parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind texture and free data
    stbi_image_free(imageData);
    stbi_set_flip_vertically_on_load(false);
    unbind();
}

void Texture::activate(const unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::destroy()
{
    debug_print("Destroyed texture: %d\n", _id);
    glDeleteTextures(1, &_id);
}

} // namespace gl

} // namespace tmig