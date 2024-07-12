#pragma once

#include <string>
#include <memory>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "tmig/camera.hpp"
#include "tmig/scene.hpp"

namespace tmig {

/// @brief GLFWWindow wrapper class
class Window {
public:
    /// @brief Default constructor
    Window();

    /// @brief Constructor
    /// @param title window title
    Window(const std::string &title);

    /// @brief Destructor
    ~Window();

    /// @brief Start rendering loop
    void start();

    /// @brief Set window title
    /// @param title new title
    void setTitle(const std::string &title);

    /// @brief Set window size
    /// @param width window width
    /// @param height window height
    void setSize(const int &width, const int &height) const;

    /// @brief Get window size
    /// @param width output width
    /// @param height output height
    void getSize(int &width, int &height) const;

    /// @brief Setup objects, called before render loop
    void setup();

    /// @brief Update objects, called once each iteration of render loop
    /// @param dt delta time
    void update(float dt);

protected:
    /// @brief Process input, called once each iteration of render loop
    /// @param dt delta time
    virtual void processInput(float dt);

    /// @brief Current window title
    std::string title;

    /// @brief Current render scene
    std::shared_ptr<Scene> currentScene;

    /// @brief Projection matrix
    glm::mat4 projection;

private:
    /// @brief GLFW window object
    GLFWwindow *window;
};

} // namespace tmig