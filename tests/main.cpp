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

    bool flashlightFollowing = true;
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
        float d = 10.0f;
        auto wallLeft = std::make_shared<Entity>(utils::boxGMesh());
        wallLeft->setScale(glm::vec3{1.0f, d * 2.0f, d * 2.0f});
        wallLeft->setPosition(glm::vec3{-d, 0.0f, 0.0f});
        // wallLeft->setColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

        auto wallRight = std::make_shared<Entity>(utils::boxGMesh());
        wallRight->setScale(glm::vec3{1.0f, d * 2.0f, d * 2.0f});
        wallRight->setPosition(glm::vec3{d, 0.0f, 0.0f});
        // wallRight->setColor(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
        wallRight->setRotation(glm::rotate(glm::mat4{1.0f}, glm::radians(180.f), glm::vec3{0.0f, 0.0f, 1.0f}));

        auto wallBack = std::make_shared<Entity>(utils::boxGMesh());
        wallBack->setScale(glm::vec3{d * 2.0f, d * 2.0f, 1.0f});
        wallBack->setPosition(glm::vec3{0.0f, 0.0f, d});
        // wallBack->setColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});

        auto wallFront = std::make_shared<Entity>(utils::boxGMesh());
        wallFront->setScale(glm::vec3{d * 2.0f, d * 2.0f, 1.0f});
        wallFront->setPosition(glm::vec3{0.0f, 0.0f, -d});
        // wallFront->setColor(glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});

        auto wallBottom = std::make_shared<Entity>(utils::boxGMesh());
        wallBottom->setScale(glm::vec3{d * 2.0f, 1.0f, d * 2.0f});
        wallBottom->setPosition(glm::vec3{0.0f, -d, 0.0f});
        // wallBottom->setColor(glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});

        auto wallTop = std::make_shared<Entity>(utils::boxGMesh());
        wallTop->setScale(glm::vec3{d * 2.0f, 1.0f, d * 2.0f});
        wallTop->setPosition(glm::vec3{0.0f, d, 0.0f});
        // wallTop->setColor(glm::vec4{1.0f, 0.0f, 1.0f, 1.0f});

        movingEntity = std::make_shared<Entity>(utils::sphereGMesh());
        movingEntity->setScale(glm::vec3{0.5f});

        rotatingEntity = std::make_shared<Entity>(utils::torusGMesh());
        // rotatingEntity->setScale(glm::vec3{1.0f, 3.0f, 1.0f});
        rotatingEntity->setScale(glm::vec3{0.5f});

        lightsScene->addEntity(wallLeft);
        lightsScene->addEntity(wallRight);
        lightsScene->addEntity(wallBack);
        lightsScene->addEntity(wallFront);
        lightsScene->addEntity(wallBottom);
        lightsScene->addEntity(wallTop);
        lightsScene->addEntity(movingEntity);
        lightsScene->addEntity(rotatingEntity);

        float s = 0.75f;
        auto light1 = std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f, 0.0f}, s, glm::vec3{0.0f, 2.0f, 0.0f});
        auto light2 = std::make_shared<PointLight>(glm::vec3{0.0f, 1.0f, 0.0f}, s, glm::vec3{0.0f, -2.0f, 0.0f});
        auto light3 = std::make_shared<PointLight>(glm::vec3{0.0f, 0.0f, 1.0f}, s, glm::vec3{2.0f, 0.0f, 0.0f});
        auto light4 = std::make_shared<PointLight>(glm::vec3{1.0f, 1.0f, 0.0f}, s, glm::vec3{-2.0f, 0.0f, 0.0f});
        auto light5 = std::make_shared<PointLight>(glm::vec3{1.0f, 0.0f, 1.0f}, s, glm::vec3{0.0f, 0.0f, 2.0f});
        auto light6 = std::make_shared<PointLight>(glm::vec3{0.0f, 1.0f, 1.0f}, s, glm::vec3{0.0f, 0.0f, -2.0f});

        lightsScene->addLight(light1);
        lightsScene->addLight(light2);
        lightsScene->addLight(light3);
        lightsScene->addLight(light4);
        lightsScene->addLight(light5);
        lightsScene->addLight(light6);

        auto lightCube1 = std::make_shared<Entity>(utils::boxGMesh());
        lightCube1->setColor(glm::vec4{light1->color, 1.0f});
        lightCube1->setPosition(glm::vec3{0.0f, 2.0f, 0.0f});
        lightCube1->setScale(glm::vec3{.2f});

        auto lightCube2 = std::make_shared<Entity>(utils::boxGMesh());
        lightCube2->setColor(glm::vec4{light2->color, 1.0f});
        lightCube2->setPosition(glm::vec3{0.0f, -2.0f, 0.0f});
        lightCube2->setScale(glm::vec3{.2f});

        auto lightCube3 = std::make_shared<Entity>(utils::boxGMesh());
        lightCube3->setColor(glm::vec4{light3->color, 1.0f});
        lightCube3->setPosition(glm::vec3{2.0f, 0.0f, 0.0f});
        lightCube3->setScale(glm::vec3{.2f});

        auto lightCube4 = std::make_shared<Entity>(utils::boxGMesh());
        lightCube4->setColor(glm::vec4{light4->color, 1.0f});
        lightCube4->setPosition(glm::vec3{-2.0f, 0.0f, 0.0f});
        lightCube4->setScale(glm::vec3{.2f});

        auto lightCube5 = std::make_shared<Entity>(utils::boxGMesh());
        lightCube5->setColor(glm::vec4{light5->color, 1.0f});
        lightCube5->setPosition(glm::vec3{0.0f, 0.0f, 2.0f});
        lightCube5->setScale(glm::vec3{.2f});

        auto lightCube6 = std::make_shared<Entity>(utils::boxGMesh());
        lightCube6->setColor(glm::vec4{light6->color, 1.0f});
        lightCube6->setPosition(glm::vec3{0.0f, 0.0f, -2.0f});
        lightCube6->setScale(glm::vec3{.2f});

        lightsScene->addEntity(lightCube1);
        lightsScene->addEntity(lightCube2);
        lightsScene->addEntity(lightCube3);
        lightsScene->addEntity(lightCube4);
        lightsScene->addEntity(lightCube5);
        lightsScene->addEntity(lightCube6);
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

    auto floor = std::make_shared<Entity>(utils::boxGMesh());
    floor->setColor(glm::vec4{1.0f, 0.5f, 0.25f, 1.0f});
    floor->setScale(glm::vec3{10.f, 1.0f, 30.0f});
    floor->setPosition(glm::vec3{0.0f, -2.0f, 0.0f});
    flashlightScene->addEntity(floor);

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

    if (currentScene == lightsScene) {
        auto t = elapsedTime();
        glm::mat4 rot{1.0f};
        rot = glm::rotate(rot, t, glm::vec3{1.0f, 0.2f, -0.7f});
        rotatingEntity->setRotation(rot);

        float s = 2.0f;
        movingEntity->setPosition(glm::vec3{
            std::cos(t) * s,
            std::cos(t * 4.0f),
            std::sin(t) * s,
        });
    } else if (flashlightFollowing) {
        flashlight->pos = currentScene->camera.pos;
        flashlight->dir = currentScene->camera.getForward();
    }
}

void App::processInput(float dt) {
    // TODO: Change this to be more understandable (base processInput() for now is just camera movement)
    Window::processInput(dt);

    // Toggle scenes when pressing F
    if (isKeyPressed(KeyCode::f)) {
        if (currentScene == flashlightScene) {
            currentScene = lightsScene;
        } else {
            currentScene = flashlightScene;
        }
    }
    if (currentScene == flashlightScene && isKeyPressed(KeyCode::g)) {
        flashlightFollowing = !flashlightFollowing;
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