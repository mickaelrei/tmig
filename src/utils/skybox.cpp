#include <stdexcept>

#include "glad/glad.h"

#include "tmig/utils/skybox.hpp"
#include "tmig/utils/primitives.hpp"
#include "tmig/utils/shaders.hpp"

namespace tmig {

namespace utils {

Skybox::Skybox(const gl::TextureCube &textureCube)
    : Entity::Entity{boxMesh},
      textureCube{textureCube}
{

}

void Skybox::draw(const gl::Shader& shader) const
{
    // TODO: This all needs to change. GLAD shouldnt need to be
    //       included in these files, instead make a tmig::enable
    //       namespace to enable culling, changing depth func, etc

    // Custom functionalities specific for skybox
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    // Bind shader and VAO
    shader.use();
    shader.setMat4("model", _modelMatrix);
    vao.bind();
    textureCube.bind();

    // Draw
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();

    // Back to defaults
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
}

} // namespace utils

} // namespace tmig