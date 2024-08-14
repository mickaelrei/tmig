#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "tmig/init.hpp"
#include "tmig/utils/primitives_gmesh.hpp"
#include "tmig/utils/shaders.hpp"
#include "tmig/window.hpp"

void printMat3(const glm::mat3 &m) {
    printf("[%.3f, %.3f, %.3f]\n", m[0][0], m[0][1], m[0][2]);
    printf("[%.3f, %.3f, %.3f]\n", m[1][0], m[1][1], m[1][2]);
    printf("[%.3f, %.3f, %.3f]\n", m[2][0], m[2][1], m[2][2]);
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

class RigidBody : public tmig::Entity {
public:
    RigidBody() = default;

    RigidBody(const glm::vec3 &scale)
        : Entity::Entity{tmig::utils::boxGMesh()} {
        setScale(scale);
    }

    void update(float dt) {
        (void)dt;

        // Update auxiliary variables
        R = glm::mat3{q};
        iInv = R * iBodyInv * glm::transpose(R);
        vel = P / mass;
        omega = iInv * L;

        // Update position
        setPosition(_position + vel);

        // Update rotation
        glm::quat qdt = 0.5f * (omega * q);
        q = glm::normalize(q * qdt);
        // glm::mat4 Rdot = star(omega) * R;
        // // Reset W component to avoid adding it to rotation
        // Rdot[3][3] = 0.0f;
        setRotation(glm::mat4{q});

        // Update linear and angular momentum
        P += force * dt;
        L += torque * dt;

        // Apply drag to reduce momentum
        P *= .99f;
        L *= .99f;

        // Reset force and torque
        force = glm::vec3{0.0f};
        torque = glm::vec3{0.0f};
    }

    void setScale(const glm::vec3 &scale) override {
        // Set scale variable
        _scale = scale;
        updateModelMatrix();
        auto scale2 = glm::pow(scale, glm::vec3{2.0f});

        // Set constants based on scale
        mass = scale.x * scale.y * scale.z;
        iBody = glm::mat3{
            scale2.y + scale2.z, 0.0f, 0.0f,
            0.0f, scale2.x + scale2.z, 0.0f,
            0.0f, 0.0f, scale2.x + scale2.y
        };
        iBodyInv = glm::inverse(iBody);
    }

    void applyForce(const glm::vec3 &force) {
        RigidBody::force += force;
    }

    void applyTorque(const glm::vec3 &torque) {
        RigidBody::torque += torque;
    }

    void applyRelativeForce(const glm::vec3 &force) {
        // TODO: Convert [force] vector to relative coordinates (use rotation quat)
        (void)force;
    }

    void applyRelativeTorque(const glm::vec3 &torque) {
        // TODO: Convert [torque] vector to relative coordinates (use rotation quat)
        (void)torque;
    }

    void applyForceAtPosition(const glm::vec3 &force, const glm::vec3 &pos) {
        // TODO: Applying a force on a specific position will apply both
        //       force and torque. Find a way of calculting these two
        (void)force;
        (void)pos;
    }

private:
    // Constants
    float mass;
    glm::mat3 iBody;
    glm::mat3 iBodyInv;

    // State variables
    glm::quat q = glm::quat{1.0f, glm::vec3{0.0f}};     // Rotation quaternion
    glm::vec3 P = glm::vec3{0.0f};                      // Linear momentum (position change)
    glm::vec3 L = glm::vec3{0.0f};                      // Angular momentum (rotation change)

    // Derived values (auxiliary)
    glm::mat3 iInv = glm::mat3{1.0f};                   // Inverse of inertia tensor
    glm::mat3 R = glm::mat3{1.0f};                      // Rotation matrix
    glm::vec3 vel = glm::vec3{0.0f};                    // velocity
    glm::vec3 omega = glm::vec3{0.0f};                  // axis of rotation

    // Computed values (external like wind, gravity, interaction with other rigid-bodies etc.)
    glm::vec3 force = glm::vec3{0.0f};                  // sum of all forces
    glm::vec3 torque = glm::vec3{0.0f};                 // sum of all torque
};

class App : public tmig::Window {
public:
    App();

    void setup() override;
    void update(float dt) override;
    void processInput(float dt) override;

    std::shared_ptr<tmig::Scene> rbScene;
    std::vector<std::shared_ptr<RigidBody>> rigidBodies;
};

App::App()
    : tmig::Window{"Verlet Physics"} {}

void App::setup() {
    using namespace tmig;

    // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // not needed, GL_BACK is the default culled/ignored face

    auto light = std::make_shared<PointLight>(glm::vec3{1.0f}, 1.0f, glm::vec3{0.0f});

    auto floor = std::make_shared<RigidBody>(glm::vec3{10.0f, 1.0f, 10.0f});
    floor->setPosition(glm::vec3{0.0f, -3.0f, 0.0f});

    auto rb = std::make_shared<RigidBody>(glm::vec3{1.0f});
    rb->setPosition(glm::vec3{3.0f, 0.0f, 0.0f});

    auto rb1 = std::make_shared<RigidBody>(glm::vec3{1.0f, 1.0f, 2.0f});
    rb1->setPosition(glm::vec3{-2.0f, 0.0f, 0.0f});

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
    rbScene->setProjection(getSize());

    // rbScene->addEntity(rb);
    // rigidBodies.push_back(rb);

    rigidBodies.push_back(rb1);
    rbScene->addEntity(rb1);
}

void App::update(float dt) {
    // Apply force and torque BEFORE updating
    if (isKeyPressed(KeyCode::r)) {
        rigidBodies[0]->applyForce(glm::vec3{0.0f, 1.0f, 0.0f});
    }

    if (isKeyPressed(KeyCode::x)) {
        rigidBodies[0]->applyTorque(glm::vec3{1.0f, 0.0f, 0.0f});
    }
    if (isKeyPressed(KeyCode::y)) {
        rigidBodies[0]->applyTorque(glm::vec3{0.0f, 1.0f, 0.0f});
    }
    if (isKeyPressed(KeyCode::z)) {
        rigidBodies[0]->applyTorque(glm::vec3{0.0f, 0.0f, 1.0f});
    }

    if (isKeyPressed(KeyCode::v)) {
        rigidBodies[0]->setScale(rigidBodies[0]->getScale() * 2.0f);
    }

    // if (isKeyPressed(KeyCode::f)) {
        for (auto &rb : rigidBodies) {
            rb->update(dt);
        }
    // }

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
    std::cout << "Verlet Physics\n";
    app->start();

    // Terminate tmig engine when app finishes running
    tmig::terminate();

    return 0;
}