#include <stdexcept>

#include "glad/glad.h"

#include "tmig/utils/skybox.hpp"
#include "tmig/utils/primitives_gmesh.hpp"

namespace tmig {

namespace utils {

Skybox::Skybox(const gl::TextureCube &textureCube)
    : Entity::Entity{boxGMesh()},
      textureCube{textureCube}
{

}

void Skybox::draw(const gl::Shader& shader) const
{
    // TODO: This all needs to change. GLAD shouldnt need to be
    //       included in these files, instead make a tmig::enable
    //       namespace to enable culling, changing depth func, etc

    // TODO: Create some sort of state/context which holds info
    //       such as current depth func, current cull face, current
    //       stencil op, current depth mask, etc. and get/setters for it

    // Custom functionalities specific for skybox
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    // Bind shader and VAO
    shader.use();
    shader.setMat4("model", _modelMatrix);
    gmesh.vao.bind();
    textureCube.bind();

    // Draw
    glDrawElements(GL_TRIANGLES, gmesh.indices.size(), GL_UNSIGNED_INT, 0);
    gmesh.vao.unbind();

    // Back to defaults
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
}

} // namespace utils

} // namespace tmig