#include <iostream>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>
#include "glad/glad.h"

#include "tmig/init.hpp"
#include "tmig/window.hpp"
#include "tmig/gl/gl.hpp"
#include "tmig/gl/shader.hpp"
#include "tmig/entity.hpp"
#include "tmig/utils/skybox.hpp"

#include "tmig/utils/primitives_gmesh.hpp"

namespace tmig {

// Window resize
static void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

Window::Window()
    : Window::Window("Tmig Engine") {}

Window::Window(const std::string &title)
    : title{title}
{
    // Create window
    window = glfwCreateWindow(600, 600, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        throw std::runtime_error{"Failed to create GLFW window\n"};
        return;
    }

    // Set callbacks
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // Initialize GLAD
    initGLAD();
}

Window::~Window()
{
    glfwDestroyWindow(window);
}

void Window::setTitle(const std::string &title)
{
    Window::title = title;
    glfwSetWindowTitle(window, title.c_str());
}

void Window::setSize(const int &width, const int &height) const
{
    glfwSetWindowSize(window, width, height);
}

void Window::getSize(int &width, int &height) const
{
    glfwGetWindowSize(window, &width, &height);
}

float Window::elapsedTime() const {
    return (float)glfwGetTime();
}

void Window::setup()
{
    
}

void Window::update(float dt)
{
    (void)dt;
}

void Window::processInput(float dt)
{
    // Check for window close
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (currentScene == nullptr) return;

    auto &cam = currentScene->camera;

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam.moveForward(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam.moveBack(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam.moveRight(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam.moveLeft(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        cam.moveUp(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        cam.moveDown(dt);
    }

    // Camera rotation
    float rx = 0.0f;
    float ry = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        ry -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        ry += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        rx += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        rx -= 1.0f;
    }

    cam.rotate(rx, ry, dt);
}

void Window::start()
{
    // Enable OpenGL functionalities
    {
        // Enable blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // not needed, GL_BACK is the default culled/ignored face
    }

    // Call setup before starting
    setup();

    if (currentScene == nullptr) {
        std::cout << "[WARNING]: No scene set!\n";
    }

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPointSize(5.0f);

    float last = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        //------------------------------------------------------
        //------------------------------------------------------
        // Clear screen

        glm::vec4 clearColor{0.05f, 0.05f, 0.05f, 1.0f};
        if (currentScene != nullptr) {
            clearColor = currentScene->clearColor;
        }
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //------------------------------------------------------
        //------------------------------------------------------
        // Calculate delta time

        float t = glfwGetTime();
        float dt = t - last;
        last = t;

        processInput(dt);
        update(dt);

        if (currentScene != nullptr) {
            //------------------------------------------------------
            //------------------------------------------------------
            // Get projection and view matrix

            int width, height;
            getSize(width, height);

            auto &cam = currentScene->camera;
            projection = glm::perspective(
                glm::radians(cam.fov),
                (float)width / (float)height,
                cam.minDist, cam.maxDist
            );

            auto view = cam.getViewMatrix();
            gl::skyboxShader().setMat4("view", glm::mat4{glm::mat3{view}});
            gl::entityShader().setMat4("view", view);

            gl::skyboxShader().setMat4("projection", projection);
            gl::entityShader().setMat4("projection", projection);

            gl::entityShader().setVec3("viewPos", currentScene->camera.pos);

            //------------------------------------------------------
            //------------------------------------------------------
            // Update scene

            currentScene->update(dt);
            currentScene->render();
        }

        //------------------------------------------------------
        //------------------------------------------------------
        // Swap buffers and poll events

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

} // namespace tmig