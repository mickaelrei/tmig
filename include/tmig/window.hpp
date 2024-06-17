#pragma once

#include <string>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "tmig/camera.hpp"
#include "tmig/scene.hpp"

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

    void setup();
    void update(float dt);

protected:
    virtual void processInput(float dt);

    std::string title;
    std::shared_ptr<Scene> currentScene;

    glm::mat4 projection;

private:
    GLFWwindow *window;
};

} // namespace tmig