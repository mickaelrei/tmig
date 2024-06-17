#pragma once

#include <string>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "scene.hpp"

namespace tmig {

class Window
{
public:
    Window(const std::string &title);
    ~Window();

    void start();

    void setTitle(const std::string &title);
    void setSize(const int &width, const int &height) const;
    void getSize(int &width, int &height) const;

private:
    void processInput(float dt);

    std::string title;
    std::shared_ptr<Scene> currentScene;

    glm::mat4 projection;

    GLFWwindow *window;
};

} // namespace tmig