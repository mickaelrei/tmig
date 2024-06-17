#include <stdexcept>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "utils/skybox.hpp"
#include "utils/primitives.hpp"
#include "utils/shaders.hpp"

namespace tmig {

namespace utils {

Skybox::Skybox(
    const std::string &rightPath,
    const std::string &leftPath,
    const std::string &topPath,
    const std::string &bottomPath,
    const std::string &frontPath,
    const std::string &backPath
)
    : Entity::Entity{boxMesh, newSkyboxShader()}
{
    // Temporary vector containing texture paths
    std::vector<std::string> facePaths{
        rightPath, leftPath,
        topPath, bottomPath,
        frontPath, backPath
    };

    // Generate texture
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

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

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::draw(const glm::mat4 &mat) const
{
    // TODO: This all needs to change. GLAD shouldnt need to be
    //       included in these files, instead make a tmig::enable
    //       namespace to enable culling, changing depth func, etc

    // Custom functionalities specific for skybox
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);


    // Bind shader and VAO
    shader.use();
    shader.setMat4("model", mat * _modelMatrix);
    vao.bind();
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    // Draw
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();

    // Back to defaults
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
}

} // namespace utils

} // namespace tmig