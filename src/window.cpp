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

void Window::setup()
{
    currentScene->camera.pos = glm::vec3{0.0f, 0.0f, 3.0f};

    gl::Texture faceTexture{"resources/textures/awesomeface.png"};
    gl::Texture crateTexture{"resources/textures/container.jpg"};

    auto wallLeft = std::make_shared<Entity>(utils::boxGMesh());
    wallLeft->setScale(glm::vec3{1.0f, 10.0f, 10.0f});
    wallLeft->setPosition(glm::vec3{-5.0f, 0.0f, 0.0f});
    // wallLeft->setColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

    auto wallRight = std::make_shared<Entity>(utils::boxGMesh());
    wallRight->setScale(glm::vec3{1.0f, 10.0f, 10.0f});
    wallRight->setPosition(glm::vec3{5.0f, 0.0f, 0.0f});
    // wallRight->setColor(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    wallRight->setRotation(glm::rotate(glm::mat4{1.0f}, glm::radians(180.f), glm::vec3{0.0f, 0.0f, 1.0f}));

    auto wallBack = std::make_shared<Entity>(utils::boxGMesh());
    wallBack->setScale(glm::vec3{10.0f, 10.0f, 1.0f});
    wallBack->setPosition(glm::vec3{0.0f, 0.0f, 5.0f});
    // wallBack->setColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});

    auto wallFront = std::make_shared<Entity>(utils::boxGMesh());
    wallFront->setScale(glm::vec3{10.0f, 10.0f, 1.0f});
    wallFront->setPosition(glm::vec3{0.0f, 0.0f, -5.0f});
    // wallFront->setColor(glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});

    auto wallBottom = std::make_shared<Entity>(utils::boxGMesh());
    wallBottom->setScale(glm::vec3{10.0f, 1.0f, 10.0f});
    wallBottom->setPosition(glm::vec3{0.0f, -5.0f, 0.0f});
    // wallBottom->setColor(glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});

    auto wallTop = std::make_shared<Entity>(utils::boxGMesh());
    wallTop->setScale(glm::vec3{10.0f, 1.0f, 10.0f});
    wallTop->setPosition(glm::vec3{0.0f, 5.0f, 0.0f});
    // wallTop->setColor(glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});

    auto cube = std::make_shared<Entity>(utils::boxGMesh());
    cube->setScale(glm::vec3{1.0f, 1.0f, 1.0f});
    cube->setPosition(glm::vec3{0.0f, 0.0f, 0.0f});
    cube->setColor(glm::vec4{1.0f, 0.25f, 0.0f, 1.0f});

    auto sphere = std::make_shared<Entity>(utils::sphereGMesh(), std::vector<gl::Texture>{crateTexture});
    sphere->setPosition(glm::vec3{-2.0f, 0.0f, 0.0f});

    auto cone = std::make_shared<Entity>(utils::coneGMesh(), std::vector<gl::Texture>{faceTexture});
    cone->setPosition(glm::vec3{2.0f, 0.0f, 0.0f});

    auto cylinder = std::make_shared<Entity>(utils::cylinderGMesh());//, std::vector<gl::Texture>{faceTexture});
    // cylinder->setPosition(glm::vec3{0.0f, 2.0f, 0.0f});
    // cylinder->setColor(glm::vec4{1.0f, 0.0f, 0.5f, 1.0f});
    cylinder->setScale(glm::vec3{1.0f, 3.0f, 1.0f});
    cylinder->rotate(glm::rotate(glm::mat4{1.0f}, glm::radians(90.0f), glm::vec3{0.0f, 0.0f, 1.0f}));

    auto wedge = std::make_shared<Entity>(utils::wedgeGMesh());
    wedge->setColor(glm::vec4{0.0f, 1.0f, 0.75f, 1.0f});
    wedge->setPosition(glm::vec3{0.0f, -2.0f, 0.0f});

    auto torus = std::make_shared<Entity>(utils::torusGMesh(), std::vector<gl::Texture>{faceTexture});
    torus->setPosition(glm::vec3{0.0f, 0.0f, -2.0f});
    torus->setColor(glm::vec4{0.25f, 0.5f, 1.0f, 1.0f});

    // currentScene->addEntity(cube);
    // currentScene->addEntity(sphere);
    // currentScene->addEntity(cone);
    currentScene->addEntity(cylinder);
    // currentScene->addEntity(wedge);
    // currentScene->addEntity(torus);

    currentScene->addEntity(wallLeft);
    currentScene->addEntity(wallRight);
    currentScene->addEntity(wallBack);
    currentScene->addEntity(wallFront);
    currentScene->addEntity(wallBottom);
    currentScene->addEntity(wallTop);

    auto light1 = std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 2.0f, 0.0f});
    auto light2 = std::make_shared<PointLight>(glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, -2.0f, 0.0f});
    auto light3 = std::make_shared<PointLight>(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{2.0f, 0.0f, 0.0f});
    auto light4 = std::make_shared<PointLight>(glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec3{-2.0f, 0.0f, 0.0f});
    auto light5 = std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 2.0f});
    auto light6 = std::make_shared<PointLight>(glm::vec3{0.0f, 1.0f, 1.0f}, glm::vec3{0.0f, 0.0f, -2.0f});
    currentScene->addLight(light1);
    currentScene->addLight(light2);
    currentScene->addLight(light3);
    currentScene->addLight(light4);
    currentScene->addLight(light5);
    currentScene->addLight(light6);
}

void Window::update(float dt)
{
    (void)dt;

    float t = glfwGetTime();

    glm::mat4 rot{1.0f};
    rot = glm::rotate(rot, t, glm::vec3{0.5f, 0.3f, 0.1f});
    currentScene->entities[0]->setRotation(rot);
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
    currentScene->setShader(gl::entityShader());
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
    glPointSize(5.0f);

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
        currentScene->render();

        //------------------------------------------------------
        //------------------------------------------------------
        // Swap buffers and poll events

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

} // namespace tmig