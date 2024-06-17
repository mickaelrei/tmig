#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "window.hpp"
#include "gl/shader.hpp"
#include "entity.hpp"
#include "utils/skybox.hpp"

#include "utils/primitives.hpp"

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
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(600, 600, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        exit(-1);
    }
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
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
    currentScene->skybox = utils::Skybox{
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };

    // Testing quads
    gl::Shader quadShader{"./resources/shaders/vertex_shader.glsl", "./resources/shaders/fragment_shader.glsl"};
    gl::Shader quadShader2{"./resources/shaders/vertex_shader.glsl", "./resources/shaders/fragment_shader2.glsl"};
    gl::Shader skyboxShader{"./resources/shaders/skybox_vert_shader.glsl", "./resources/shaders/skybox_frag_shader.glsl"};

    gl::Shader noTextureShader{"./resources/shaders/vertex_shader.glsl", "./resources/shaders/no_tex_frag_shader.glsl"};

    gl::Texture faceTexture{"resources/textures/awesomeface.png"};
    gl::Texture containerTexture{"resources/textures/container.jpg"};
    gl::Texture skyTexture{"resources/textures/skybox/back.jpg"};

    Mesh quadMesh{
        {
            Vertex{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec2{0.0f, 0.0f}},
            Vertex{glm::vec3{ 1.0f, -1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}},
            Vertex{glm::vec3{ 1.0f,  1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}},
            Vertex{glm::vec3{-1.0f,  1.0f, 0.0f}, glm::vec2{0.0f, 1.0f}},
        },
        {
            0, 1, 2,
            0, 2, 3,
        },
    };

    auto cube = std::make_shared<Entity>(tmig::utils::boxMesh, quadShader, std::vector<gl::Texture>{containerTexture});
    cube->translate(glm::vec3{1.5f, 1.5f, -7.0f});
    cube->setScale(glm::vec3{3.0f, 0.5f, 2.0f});
    currentScene->addEntity(cube);

    auto sphere = std::make_shared<Entity>(tmig::utils::sphereMesh(), quadShader2, std::vector<gl::Texture>{containerTexture, faceTexture});
    sphere->translate(glm::vec3{0.0f, 0.0f, -3.0f});
    currentScene->addEntity(sphere);

    auto entityFace = std::make_shared<Entity>(quadMesh, quadShader, std::vector<gl::Texture>{faceTexture});
    entityFace->translate(glm::vec3{-1.5f, -1.5f, -5.0f});
    currentScene->addEntity(entityFace);

    auto entityContainerFace = std::make_shared<Entity>(quadMesh, quadShader2, std::vector<gl::Texture>{containerTexture, faceTexture});
    entityContainerFace->translate(glm::vec3{1.5f, -1.5f, -5.0f});
    currentScene->addEntity(entityContainerFace);

    auto entityContainer = std::make_shared<Entity>(quadMesh, quadShader, std::vector<gl::Texture>{containerTexture});
    entityContainer->translate(glm::vec3{-1.5f, 1.5f, -5.0f});
    currentScene->addEntity(entityContainer);

    auto entityFaceContainer = std::make_shared<Entity>(quadMesh, quadShader2, std::vector<gl::Texture>{faceTexture, containerTexture});
    entityFaceContainer->translate(glm::vec3{1.5f, 1.5f, -5.0f});
    currentScene->addEntity(entityFaceContainer);

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
        // Get delta time

        float t = glfwGetTime();
        float dt = t - last;
        last = t;
        processInput(dt);

        //------------------------------------------------------
        //------------------------------------------------------
        // Temporary "scene update" function

        float red = std::sin(t);
        sphere->setColor(glm::vec4{red, 0.5f, 0.5f, 1.0f});

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

        //------------------------------------------------------
        //------------------------------------------------------
        // Update scene

        // TODO: Make a UBO for fixed projection/view matrix uniforms
        currentScene->setProjection(projection);
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