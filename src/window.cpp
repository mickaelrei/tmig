#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "window.hpp"
#include "shader.hpp"

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.moveForward(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.moveBack(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.moveRight(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.moveLeft(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.moveUp(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.moveDown(dt);
    }

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

    camera.rotate(rx, ry, dt);
}

void Window::start()
{
    Shader containerShader{"./resources/shaders/vertex_shader.glsl", "./resources/shaders/fragment_shader.glsl"};
    Shader skyboxShader{"./resources/shaders/skybox_vert_shader.glsl", "./resources/shaders/skybox_frag_shader.glsl"};

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create textures
    stbi_set_flip_vertically_on_load(true);
    unsigned int containerTexture, faceTexture, skyboxTexture;
    glGenTextures(1, &containerTexture);
    glGenTextures(1, &faceTexture);
    glGenTextures(1, &skyboxTexture);

    // Container texture
    {
        // Bind texture
        glBindTexture(GL_TEXTURE_2D, containerTexture);

        int width, height, numChannels;
        unsigned char *imageData = stbi_load("resources/textures/container.jpg", &width, &height, &numChannels, 0);
        if (imageData == NULL)
        {
            std::cout << "Failed to load texture\n";
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(imageData);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Face texture
    {
        // Bind texture
        glBindTexture(GL_TEXTURE_2D, faceTexture);

        // Load image
        int width, height, numChannels;
        unsigned char *imageData = stbi_load("resources/textures/awesomeface.png", &width, &height, &numChannels, 0);
        if (imageData == NULL)
        {
            std::cout << "Failed to load texture\n";
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(imageData);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Skybox texture
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

        std::string faces[] = {
            "resources/textures/skybox/right.jpg",
            "resources/textures/skybox/left.jpg",
            "resources/textures/skybox/top.jpg",
            "resources/textures/skybox/bottom.jpg",
            "resources/textures/skybox/front.jpg",
            "resources/textures/skybox/back.jpg"
        };

        int width, height, nrChannels;
        unsigned char *data;
        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < sizeof(faces) / sizeof(std::string); ++i)
        {
            data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            }

            stbi_image_free(data);
        }
        stbi_set_flip_vertically_on_load(true);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // Default vertices for cube
    const float containerVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };

    // Vertices for skybox
    const float skyboxVertices[] = {
        // positions          
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
    };

    const unsigned int skyboxIndices[] = {
        // back
        4, 6, 7,
        4, 5, 6,

        // right
        5, 2, 6,
        5, 1, 2,

        // left
        0, 7, 3,
        0, 4, 7,

        // front
        1, 3, 2,
        1, 0, 3,

        // top
        7, 2, 3,
        7, 6, 2,

        // bottom
        0, 5, 4,
        0, 1, 5,
    };

    // Wood container
    unsigned int containerVBO, containerVAO;
    {
        // Create vertex objects
        glGenVertexArrays(1, &containerVAO);
        glGenBuffers(1, &containerVBO);

        // Bind the array (VAO) first
        glBindVertexArray(containerVAO);

        // Then bind and set the buffer (VBO)
        glBindBuffer(GL_ARRAY_BUFFER, containerVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW);

        // How to interpret the vertex data (layout location on vertex shader)
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    // Skybox
    unsigned int skyboxVBO, skyboxVAO, skyboxEBO;
    {
        // Create vertex objects
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glGenBuffers(1, &skyboxEBO);

        // Bind the array (VAO) first
        glBindVertexArray(skyboxVAO);

        // Then bind and set the buffer (VBO)
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

        // Then bind and set the elements buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

        // How to interpret the vertex data (layout location on vertex shader)
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    // Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // not needed, GL_BACK is the default culled/ignored face

    // Set shader texture positions
    containerShader.use();
    containerShader.setInt("texture1", 0);
    containerShader.setInt("texture2", 1);

    skyboxShader.use();
    // skyboxShader.setInt("texture1", 0);

    float last = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float t = glfwGetTime();
        float dt = t - last;
        last = t;

        processInput(dt);

        // Get projection and view matrix
        int width, height;
        getSize(width, height);
        auto projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.01f, 1000.0f);
        auto view = camera.getViewMatrix();
        

        //------------------------------------------------------
        //------------------------------------------------------

        // Draw skybox
        skyboxShader.use();
        skyboxShader.setMat4("view", glm::mat4{glm::mat3{view}});
        skyboxShader.setMat4("projection", projection);

        glCullFace(GL_FRONT);
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        //------------------------------------------------------
        //------------------------------------------------------

        // Draw container
        glm::mat4 model{1.0f};
        model = glm::translate(model, glm::vec3{0.0f, 0.0f, -3.0f});

        containerShader.use();
        containerShader.setMat4("projection", projection);
        containerShader.setMat4("view", view);
        containerShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, containerTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, faceTexture);

        glCullFace(GL_BACK);
        glBindVertexArray(containerVAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(containerVertices) / (sizeof(float) * 5UL));
        glBindVertexArray(0);

        //------------------------------------------------------
        //------------------------------------------------------

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}