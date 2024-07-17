#include <iostream>

#include "tmig/init.hpp"
#include "tmig/light.hpp"
#include "tmig/gl/gl.hpp"
#include "tmig/utils/primitives_gmesh.hpp"
#include "tmig/window.hpp"
#include "tmig/glm.hpp"

class App : public tmig::Window {
public:
    void setup() override;

    void update(float dt) override;

    void processInput(float dt) override;

private:
    std::shared_ptr<tmig::Scene> flashlightScene;
    std::shared_ptr<tmig::SpotLight> flashlight;

    std::shared_ptr<tmig::Scene> lightsScene;
    std::shared_ptr<tmig::Entity> movingEntity;
    std::shared_ptr<tmig::Entity> rotatingEntity;
};

void App::setup() {
    printf("my setup called\n");
    using namespace tmig;

    // ----------------------------------------------------------
    // ----------------------------------------------------------
    // Create scene testing multiple lights
    lightsScene = std::make_shared<tmig::Scene>();
    lightsScene->camera.pos = glm::vec3{0.0f, 0.0f, 3.0f};
    lightsScene->setShader(gl::entityShader());
    lightsScene->skybox = utils::Skybox{gl::TextureCube{
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    }};

    gl::Texture crateTexture{"resources/textures/container.jpg"};

    // Create entities to scene
    {
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

        movingEntity = std::make_shared<Entity>(utils::sphereGMesh());
        movingEntity->setScale(glm::vec3{0.5f});

        rotatingEntity = std::make_shared<Entity>(utils::torusGMesh());
        rotatingEntity->setScale(glm::vec3{0.5f});

        lightsScene->addEntity(wallLeft);
        lightsScene->addEntity(wallRight);
        lightsScene->addEntity(wallBack);
        lightsScene->addEntity(wallFront);
        lightsScene->addEntity(wallBottom);
        lightsScene->addEntity(wallTop);
        lightsScene->addEntity(movingEntity);
        lightsScene->addEntity(rotatingEntity);

        auto light1 = std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f, 0.0f}, 0.5f, glm::vec3{0.0f, 2.0f, 0.0f});
        auto light2 = std::make_shared<PointLight>(glm::vec3{0.0f, 1.0f, 0.0f}, 0.5f, glm::vec3{0.0f, -2.0f, 0.0f});
        auto light3 = std::make_shared<PointLight>(glm::vec3{0.0f, 0.0f, 1.0f}, 0.5f, glm::vec3{2.0f, 0.0f, 0.0f});
        auto light4 = std::make_shared<PointLight>(glm::vec3{1.0f, 1.0f, 0.0f}, 0.5f, glm::vec3{-2.0f, 0.0f, 0.0f});
        auto light5 = std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f, 1.0f}, 0.5f, glm::vec3{0.0f, 0.0f, 2.0f});
        auto light6 = std::make_shared<PointLight>(glm::vec3{0.0f, 1.0f, 1.0f}, 0.5f, glm::vec3{0.0f, 0.0f, -2.0f});

        lightsScene->addLight(light1);
        lightsScene->addLight(light2);
        lightsScene->addLight(light3);
        lightsScene->addLight(light4);
        lightsScene->addLight(light5);
        lightsScene->addLight(light6);
    }

    // ----------------------------------------------------------
    // ----------------------------------------------------------
    // Create scene testing spotlight as a flashlight
    flashlightScene = std::make_shared<tmig::Scene>();
    flashlightScene->camera.pos = glm::vec3{0.0f, 0.0f, 2.0f};
    flashlightScene->setShader(gl::entityShader());
    flashlightScene->renderSkybox = false;

    flashlight = std::make_shared<SpotLight>(
        glm::vec3{1.0f, 1.0f, 1.0f},
        2.5f,
        glm::vec3{0.0f, 0.0f, 2.0f},
        glm::vec3{0.0f, 0.0f, -1.0f},
        glm::radians(12.5f),
        glm::radians(17.5f)
    );
    flashlightScene->addLight(flashlight);

    glm::vec3 pos[] = {
        {0.7f, 0.6f, -1.0f},
        {-0.7f, -.5f, -3.0f},
        {0.0f, 1.6f, -4.0f},
        {1.2f, 1.0f, -5.0f},
        {0.7f, -0.6f, -4.0f},
        {-1.0f, 1.3f, -8.0f},
    };

    glm::mat4 rot[] = {
        glm::rotate(glm::mat4{1.0f}, glm::radians(45.0f), glm::vec3{1.0f, 0.0f, 0.5f}),
        glm::rotate(glm::mat4{1.0f}, glm::radians(90.0f), glm::vec3{-0.3f, 1.0f, 0.2f}),
        glm::rotate(glm::mat4{1.0f}, glm::radians(27.0f), glm::vec3{0.4f, -0.9f, 0.9f}),
        glm::rotate(glm::mat4{1.0f}, glm::radians(58.0f), glm::vec3{1.6f, -0.3f, 0.0f}),
        glm::rotate(glm::mat4{1.0f}, glm::radians(43.0f), glm::vec3{0.8f, 0.5f, -0.4f}),
        glm::rotate(glm::mat4{1.0f}, glm::radians(184.0f), glm::vec3{0.1f, 0.9f, -0.8f}),
    };

    for (size_t i = 0; i < 6; ++i) {
        auto box = std::make_shared<Entity>(utils::boxGMesh(), std::vector<gl::Texture>{crateTexture});
        box->setPosition(pos[i]);
        box->setRotation(rot[i]);
        flashlightScene->addEntity(box);
    }

    // Start with lights scene
    currentScene = lightsScene;
}

void App::update(float dt) {
    (void)dt;

    auto pos = currentScene->camera.pos;
    printf("%.2f, %.2f, %.2f\n", pos.x, pos.y, pos.z);

    if (currentScene == lightsScene) {
        auto t = elapsedTime();
        glm::mat4 rot{1.0f};
        rot = glm::rotate(rot, t, glm::vec3{1.0f, 0.2f, -0.7f});
        rotatingEntity->setRotation(rot);

        float s = 2.0f;
        movingEntity->setPosition(glm::vec3{
            std::cos(t) * s,
            0.0f,
            std::sin(t) * s,
        });
    } else {
        flashlight->pos = currentScene->camera.pos;
        flashlight->dir = currentScene->camera.getForward();
    }
}

void App::processInput(float dt) {
    // TODO: Change this to be more understandable (base processInput() for now is just camera movement)
    Window::processInput(dt);

    // Toggle scenes when pressing F
    // TODO: Change this to a wrapper "getKeyState(tmig::Window::Key)"
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (currentScene == flashlightScene) {
            currentScene = lightsScene;
        } else {
            currentScene = flashlightScene;
        }
    }
}

int main()
{
    // Initialize tmig engine
    tmig::init();

    // Create app
    std::shared_ptr<App> app{new App()};

    // Run
    std::cout << "Two scenes test\n";
    app->start();

    // Terminate tmig engine when app finishes running
    tmig::terminate();

    return 0;
}