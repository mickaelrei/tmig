#include <iostream>

#include "tmig/init.hpp"
#include "tmig/physics/rigid-body.hpp"
#include "tmig/render/utils/primitives_gmesh.hpp"
#include "tmig/render/utils/shaders.hpp"
#include "tmig/render/window.hpp"

void printMat3(const glm::mat3 &m) {
    printf("[%.3f, %.3f, %.3f]\n", m[0][0], m[0][1], m[0][2]);
    printf("[%.3f, %.3f, %.3f]\n", m[1][0], m[1][1], m[1][2]);
    printf("[%.3f, %.3f, %.3f]\n", m[2][0], m[2][1], m[2][2]);
}

void printMat4(const glm::mat4 &m) {
    printf("[%.3f, %.3f, %.3f, %.3f]\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    printf("[%.3f, %.3f, %.3f, %.3f]\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    printf("[%.3f, %.3f, %.3f, %.3f]\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    printf("[%.3f, %.3f, %.3f, %.3f]\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

void printVec3(const glm::vec3 &v) {
    printf("[%.3f, %.3f, %.3f]\n", v.x, v.y, v.z);
}

glm::mat3 star(const glm::vec3 &v) {
    return glm::mat3{
         0.0f, -v.z,   v.y,
         v.z,   0.0f, -v.x,
        -v.y,   v.x,   0.0f
    };
}

class App : public tmig::render::Window {
public:
    App();

    glm::quat a;

    void setup() override;
    void update(float dt) override;
    void processInput(float dt) override;

    std::shared_ptr<tmig::render::Scene> rbScene;
    std::vector<std::shared_ptr<tmig::physics::RigidBody>> rigidBodies;

    std::shared_ptr<tmig::render::Entity> sphere;
};

App::App()
    : tmig::render::Window{"Verlet Physics"} {}

void App::setup() {
    using namespace tmig::render;
    using namespace tmig::physics;

    // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // not needed, GL_BACK is the default culled/ignored face

    auto light = std::make_shared<PointLight>(glm::vec3{1.0f}, 1.0f, glm::vec3{5.0f, 5.0f, 5.0f});

    sphere = std::make_shared<Entity>(utils::sphereGMesh());
    sphere->setColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    sphere->setScale(glm::vec3{0.2f});

    auto floor = std::make_shared<RigidBody>(glm::vec3{10.0f, 1.0f, 10.0f});
    floor->setPosition(glm::vec3{0.0f, -3.0f, 0.0f});

    auto rb = std::make_shared<RigidBody>(glm::vec3{1.0f});
    rb->setPosition(glm::vec3{3.0f, 0.0f, 0.0f});

    auto rb1 = std::make_shared<RigidBody>(glm::vec3{12.0f, 1.0f, 4.0f});
    rb1->setPosition(glm::vec3{-15.0f, 0.0f, 0.0f});
    rb1->setRotation(glm::rotate(glm::mat4{1.0f}, glm::radians(90.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
    rb1->applyForce(glm::vec3{1.0f, 0.0f, -1.0f} * 48.0f);
    rb1->applyTorque(glm::vec3{2.5f, 3.3f, 4.6f} * 48.0f * 3.0f);

    rbScene = std::make_shared<Scene>();
    rbScene->camera.pos = glm::vec3{0.0f, 5.0f, 5.0f};
    rbScene->camera.rotate(-45.0f / rbScene->camera.rotationSpeed, 0.0f);
    rbScene->setShader(utils::entityShader());
    rbScene->skybox = utils::Skybox{gl::TextureCube::create(
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    )};

    currentScene = rbScene;
    rbScene->addEntity(floor);
    rbScene->addLight(light);

    // rbScene->addEntity(rb);
    // rigidBodies.push_back(rb);

    rigidBodies.push_back(rb1);
    rbScene->addEntity(rb1);

    rbScene->addEntity(sphere);
}

void App::update(float dt) {
    // Move in local right direction
    if (isKeyHeld(KeyCode::r)) {
        rigidBodies[0]->applyRelativeForce(glm::vec3{1.0f, 0.0f, 0.0f});
    }

    // Rotation on each local axis
    if (isKeyHeld(KeyCode::x)) {
        rigidBodies[0]->applyRelativeTorque(glm::vec3{1.0f, 0.0f, 0.0f});
    }
    if (isKeyHeld(KeyCode::y)) {
        rigidBodies[0]->applyRelativeTorque(glm::vec3{0.0f, 1.0f, 0.0f});
    }
    if (isKeyHeld(KeyCode::z)) {
        rigidBodies[0]->applyRelativeTorque(glm::vec3{0.0f, 0.0f, 1.0f});
    }

    // Apply force at position
    if (isKeyPressed(KeyCode::g)) {
        auto& body = rigidBodies[0];
        float f = 70.0f;

        body->applyForceAtPosition(
            body->vectorToWorldSpace(glm::vec3{0.0f, 0.0f, -f}),
            body->pointToWorldSpace(glm::vec3{-3.0f, 0.0f,  2.0f})
        );

        body->applyForceAtPosition(
            body->vectorToWorldSpace(glm::vec3{0.0f, 0.0f, f}),
            body->pointToWorldSpace(glm::vec3{3.0f, 0.0f,  -2.0f})
        );
    }

    if (isKeyPressed(KeyCode::v)) {
        rigidBodies[0]->setScale(rigidBodies[0]->getScale() * 2.0f);
    }

    // Fixed simulation step time
    const float dtSimulation = 0.001f;
    float simulatedTime = 0.0f;
    do {
        for (auto &rb : rigidBodies) {
            rb->update(dtSimulation);
        }
        simulatedTime += dtSimulation;
    } while (simulatedTime < dt);

    // Testing toLocal and toWorld methods
    auto pos = glm::vec3{-6.0f, 0.5f, 2.0f};
    pos = rigidBodies[0]->pointToWorldSpace(pos);
    pos = rigidBodies[0]->pointToLocalSpace(pos);
    pos = rigidBodies[0]->pointToWorldSpace(pos);
    sphere->setPosition(pos);

    currentScene->setProjection(getSize());
    rbScene->update(dt);
    rbScene->render();
}

void App::processInput(float dt) {
    Window::processInput(dt);
}

int main()
{
    // Initialize tmig engine
    tmig::init();

    // Create app
    auto app = std::make_shared<App>();

    // Run
    std::cout << "Rigid Body Physics\n";
    app->start();

    // Terminate tmig engine when app finishes running
    tmig::terminate();

    return 0;
}