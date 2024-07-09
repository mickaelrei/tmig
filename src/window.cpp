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

#include "tmig/utils/primitives.hpp"

namespace tmig {

// Window resize
static void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

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

void Window::setup()
{
    currentScene->camera.pos = glm::vec3{0.0f, 0.0f, 3.0f};

    auto wallLeft = std::make_shared<Entity>(utils::boxMesh);
    wallLeft->setScale(glm::vec3{1.0f, 10.0f, 10.0f});
    wallLeft->setPosition(glm::vec3{-5.0f, 0.0f, 0.0f});
    wallLeft->setColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

    auto wallRight = std::make_shared<Entity>(utils::boxMesh);
    wallRight->setScale(glm::vec3{1.0f, 10.0f, 10.0f});
    wallRight->setPosition(glm::vec3{5.0f, 0.0f, 0.0f});
    wallRight->setColor(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    wallRight->setRotation(glm::rotate(glm::mat4{1.0f}, glm::radians(45.f), glm::vec3{0.0f, 0.0f, 1.0f}));

    auto wallBack = std::make_shared<Entity>(utils::boxMesh);
    wallBack->setScale(glm::vec3{10.0f, 10.0f, 1.0f});
    wallBack->setPosition(glm::vec3{0.0f, 0.0f, 5.0f});
    wallBack->setColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});

    auto wallFront = std::make_shared<Entity>(utils::boxMesh);
    wallFront->setScale(glm::vec3{10.0f, 10.0f, 1.0f});
    wallFront->setPosition(glm::vec3{0.0f, 0.0f, -5.0f});
    wallFront->setColor(glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});

    auto wallBottom = std::make_shared<Entity>(utils::boxMesh);
    wallBottom->setScale(glm::vec3{10.0f, 1.0f, 10.0f});
    wallBottom->setPosition(glm::vec3{0.0f, -5.0f, 0.0f});
    wallBottom->setColor(glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});

    auto wallTop = std::make_shared<Entity>(utils::boxMesh);
    wallTop->setScale(glm::vec3{10.0f, 1.0f, 10.0f});
    wallTop->setPosition(glm::vec3{0.0f, 5.0f, 0.0f});
    wallTop->setColor(glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});

    currentScene->addEntity(wallLeft);
    currentScene->addEntity(wallRight);
    currentScene->addEntity(wallBack);
    currentScene->addEntity(wallFront);
    currentScene->addEntity(wallBottom);
    currentScene->addEntity(wallTop);

    auto cube = std::make_shared<Entity>(utils::boxMesh);
    cube->setScale(glm::vec3{1.0f, 1.0f, 1.0f});
    cube->setPosition(glm::vec3{0.0f, 0.0f, 0.0f});
    cube->setColor(glm::vec4{1.0f, 0.25f, 0.0f, 1.0f});

    currentScene->addEntity(cube);
}

void Window::update(float dt)
{
    (void)dt;

    float t = glfwGetTime();
    float s = 4.5f;
    float e = 0.5f;
    gl::entityShader().setVec3(
        "lightPos",
        glm::vec3{std::cos(t * e) * s, 0.0f, std::sin(t * e) * s}
    );
}

void Window::processInput(float dt)
{
    auto &cam = currentScene->camera;

    // Check for window close
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

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
    // Create new scene
    currentScene = std::make_shared<Scene>();
    currentScene->skybox = utils::Skybox{gl::TextureCube{
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    }};

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

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float last = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        //------------------------------------------------------
        //------------------------------------------------------
        // Clear screen

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //------------------------------------------------------
        //------------------------------------------------------
        // Calculate delta time

        float t = glfwGetTime();
        float dt = t - last;
        last = t;
        processInput(dt);

        //------------------------------------------------------
        //------------------------------------------------------
        // Call update

        update(dt);

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

        // TODO: Make a UBO for fixed projection/view matrix uniforms
        currentScene->update(dt);
        currentScene->render(gl::entityShader());

        //------------------------------------------------------
        //------------------------------------------------------
        // Swap buffers and poll events

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

} // namespace tmig