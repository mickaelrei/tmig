#include <stdexcept>
#include <vector>

#include "glad/glad.h"
#include "stb/stb_image.h"

#include "tmig/gl/texture_cube.hpp"

namespace tmig {

namespace gl {

std::shared_ptr<TextureCube> TextureCube::create(
    const std::string &rightPath,
    const std::string &leftPath,
    const std::string &topPath,
    const std::string &bottomPath,
    const std::string &frontPath,
    const std::string &backPath
) {
    return std::shared_ptr<TextureCube>{new TextureCube{
        rightPath,
        leftPath,
        topPath,
        bottomPath,
        frontPath,
        backPath
    }, Deleter{}};
}

TextureCube::TextureCube(
    const std::string &rightPath,
    const std::string &leftPath,
    const std::string &topPath,
    const std::string &bottomPath,
    const std::string &frontPath,
    const std::string &backPath
)
{
    // Temporary vector containing texture paths
    std::vector<std::string> facePaths{
        rightPath, leftPath,
        topPath, bottomPath,
        frontPath, backPath
    };

    // Generate texture
    glGenTextures(1, &_id);
    printf("Created texture cube id %d\n", _id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

    // Load images
    int width, height, nrChannels;
    unsigned char *data;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < 6; ++i)
    {
        data = stbi_load(facePaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            throw std::invalid_argument{"Could not load image: " + facePaths[i]};

        }

        stbi_image_free(data);
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Unbind
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCube::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
}

void TextureCube::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCube::destroy()
{
    printf("destroy texture cube id %d\n", _id);
    glDeleteTextures(1, &_id);
}

} // namespace gl

} // namespace tmig