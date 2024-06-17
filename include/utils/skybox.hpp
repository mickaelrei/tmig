#pragma once

#include "entity.hpp"

namespace tmig {

namespace utils {

class Skybox : public Entity
{
public:
    Skybox() = default;
    Skybox(
        const std::string &rightPath,
        const std::string &leftPath,
        const std::string &topPath,
        const std::string &bottomPath,
        const std::string &frontPath,
        const std::string &backPath
    );

    void draw(const glm::mat4 &mat = glm::mat4{1.0f}) const override;

private:
    unsigned int id;
};

} // namespace utils

} // namespace tmig