#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "window.hpp"
#include "gl/shader.hpp"
#include "entity.hpp"

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
    Shader quadShader{"./resources/shaders/vertex_shader.glsl", "./resources/shaders/fragment_shader.glsl"};
    Shader quadShader2{"./resources/shaders/vertex_shader.glsl", "./resources/shaders/fragment_shader2.glsl"};
    Shader skyboxShader{"./resources/shaders/skybox_vert_shader.glsl", "./resources/shaders/skybox_frag_shader.glsl"};

    stbi_set_flip_vertically_on_load(true);
    Texture faceTexture{"resources/textures/awesomeface.png"};
    Texture containerTexture{"resources/textures/container.jpg"};

    Mesh quadMesh{
        std::vector<Vertex>{
            Vertex{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec2{0.0f, 0.0f}},
            Vertex{glm::vec3{1.0f, -1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}},
            Vertex{glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}},
            Vertex{glm::vec3{-1.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 1.0f}},
        },
        std::vector<unsigned int>{
            0, 1, 2,
            0, 2, 3,
        },
    };
    Entity entityFace{quadMesh, quadShader, std::vector<Texture>{faceTexture}};
    entityFace.translate(glm::vec3{-1.5f, -1.5f, -5.0f});

    Entity entityContainerFace{quadMesh, quadShader2, std::vector<Texture>{containerTexture, faceTexture}};
    entityContainerFace.translate(glm::vec3{1.5f, -1.5f, -5.0f});

    Entity entityContainer{quadMesh, quadShader, std::vector<Texture>{containerTexture}};
    entityContainer.translate(glm::vec3{-1.5f, 1.5f, -5.0f});

    Entity entityFaceContainer{quadMesh, quadShader2, std::vector<Texture>{faceTexture, containerTexture}};
    entityFaceContainer.translate(glm::vec3{1.5f, 1.5f, -5.0f});

    // Create skybox texture (its a cubemap, so normal Texture class wont work)
    unsigned int skyboxTexture;
    glGenTextures(1, &skyboxTexture);

    // Skybox texture
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

        std::string faces[] = {
            "resources/textures/skybox/right.jpg",
            "resources/textures/skybox/left.jpg",
            "resources/textures/skybox/top.jpg",
            "resources/textures/skybox/bottom.jpg",
            "resources/textures/skybox/front.jpg",
            "resources/textures/skybox/back.jpg"};

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
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

    // Default vertices for cube (this doesnt use EBO, so normal Entity class wont work)
    // TODO: Create other entity class which doesnt use EBO, only VBO (or add some useEBO as parameter)
    //       The difference would be to call glDrawArrays() instead of glDrawElements()
    // const float containerVertices[] = {
    //     // Back face
    //     -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // Bottom-left
    //      0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right
    //      0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // bottom-right
    //      0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right
    //     -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // bottom-left
    //     -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left
    //     // Front face
    //     -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-left
    //      0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    //      0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // top-right
    //      0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // top-right
    //     -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // top-left
    //     -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-left
    //     // Left face
    //     -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // top-right
    //     -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-left
    //     -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, // bottom-left
    //     -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, // bottom-left
    //     -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-right
    //     -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // top-right
    //     // Right face
    //      0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // top-left
    //      0.5f, -0.5f, -0.5f,    0.0f, 1.0f, // bottom-right
    //      0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right
    //      0.5f, -0.5f, -0.5f,    0.0f, 1.0f, // bottom-right
    //      0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // top-left
    //      0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-left
    //     // Bottom face
    //     -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, // top-right
    //      0.5f, -0.5f, -0.5f,    1.0f, 1.0f, // top-left
    //      0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-left
    //      0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-left
    //     -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-right
    //     -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, // top-right
    //     // Top face
    //     -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left
    //      0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    //      0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right
    //      0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    //     -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left
    //     -0.5f,  0.5f,  0.5f,    0.0f, 0.0f  // bottom-left
    // };

    // Vertices for skybox (this uses vertices with only position, so using Vertex with pos and uv wont work)
    // TODO: Change entity so it can receive any type of Vertex, and create a setup() on Vertex which sets vertex attribs properly
    //       This would also need a whichFaceToRender parameter, as the skybox only renders the back face (glCullFace(GL_FRONT))
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

    // Indices for skybox
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

        // Unbind buffers
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // not needed, GL_BACK is the default culled/ignored face

    skyboxShader.use();
    skyboxShader.setInt("texture1", 0);

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
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glCullFace(GL_BACK);
        glDepthFunc(GL_LESS);

        //------------------------------------------------------
        //------------------------------------------------------

        // Set shaders matrices
        // TODO: Make a UBO for fixed projection/view matrix uniforms
        quadShader.use();
        quadShader.setMat4("projection", projection);
        quadShader.setMat4("view", view);

        quadShader2.use();
        quadShader2.setMat4("projection", projection);
        quadShader2.setMat4("view", view);

        // Draw containers with one/two textures
        entityFace.draw();
        entityContainer.draw();
        entityFaceContainer.draw();
        entityContainerFace.draw();

        //------------------------------------------------------
        //------------------------------------------------------

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}