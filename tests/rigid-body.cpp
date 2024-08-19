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

class RigidBody : public tmig::Entity {
public:
    /// @brief Default constructor (does nothing)
    RigidBody() = default;

    /// @brief Constructor with scale
    /// @param scale initial body scale
    RigidBody(const glm::vec3 &scale)
        : Entity::Entity{tmig::utils::boxGMesh()} {
        setScale(scale);

        // Create rotation axes entities
        auto s = 0.1f;
        axisX = std::make_shared<Entity>(tmig::utils::boxGMesh());
        axisX->setColor(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        axisX->setScale(glm::vec3{1.0f, s, s});

        axisY = std::make_shared<Entity>(tmig::utils::boxGMesh());
        axisY->setColor(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
        axisY->setScale(glm::vec3{s, 1.0f, s});

        axisZ = std::make_shared<Entity>(tmig::utils::boxGMesh());
        axisZ->setColor(glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
        axisZ->setScale(glm::vec3{s, s, 1.0f});
    }

    /// @brief Updates body data
    /// @param dt time between frames in secons
    void update(float dt) {
        // Update auxiliary variables
        R = glm::mat3{rot};
        iInv = R * iBodyInv * glm::transpose(R);
        vel = linearMomentum / mass;
        omega = iInv * angularMomentum;

        // Update position
        setPosition(_position + vel);

        // Update rotation
        glm::quat qdt = 0.5f * (omega * rot);
        rot = glm::normalize(rot * qdt);
        Entity::setRotation(glm::mat4{rot});

        // Update linear and angular momentum
        linearMomentum += totalForce * dt;
        angularMomentum += totalTorque * dt;

        // Apply drag to reduce momentum
        linearMomentum *= 1.0f - linearDrag * dt;
        angularMomentum *= 1.0f - angularDrag * dt;

        // Reset force and torque
        totalForce = glm::vec3{0.0f};
        totalTorque = glm::vec3{0.0f};
    }

    /// @brief Sets new rotation for rigid body
    /// @param rotation new rotation matrix
    void setRotation(const glm::mat4 &rotation) override {
        rot = glm::quat{rotation};
        Entity::setRotation(rotation);
    }

    /// @brief Sets new scale for rigid body (recalculates data)
    /// @param scale new scale vector
    void setScale(const glm::vec3 &scale) override {
        // Set scale variable
        _scale = scale;
        updateModelMatrix();
        auto scale2 = glm::pow(scale, glm::vec3{2.0f});

        // Set constants based on scale
        mass = scale.x * scale.y * scale.z;
        auto iBody = mass / 12.0f * glm::mat3{
            scale2.y + scale2.z, 0.0f, 0.0f,
            0.0f, scale2.x + scale2.z, 0.0f,
            0.0f, 0.0f, scale2.x + scale2.y
        };
        iBodyInv = glm::inverse(iBody);
    }

    /// @brief Draws rigid body and axis for visualization
    /// @param shader shader to use for rendering
    void draw(const tmig::gl::Shader &shader) const override {
        Entity::draw(shader);

        // Correct position and rotation for each axis entity
        auto rotation = getRotation();
        axisX->setRotation(rotation);
        axisY->setRotation(rotation);
        axisZ->setRotation(rotation);

        auto pos = getPosition();
        axisX->setPosition(pos - glm::mat3{rotation} * glm::vec3{-0.5f,  0.0f,  0.0f});
        axisY->setPosition(pos - glm::mat3{rotation} * glm::vec3{ 0.0f, -0.5f,  0.0f});
        axisZ->setPosition(pos - glm::mat3{rotation} * glm::vec3{ 0.0f,  0.0f, -0.5f});

        glDisable(GL_DEPTH_TEST);
        // Draw rotation axes
        axisX->draw(shader);
        axisY->draw(shader);
        axisZ->draw(shader);

        glEnable(GL_DEPTH_TEST);
    }

    /// @brief Applies a force at the center of the body
    /// @param force force vector in world space
    void applyForce(const glm::vec3 &force) {
        totalForce += force;
    }

    /// @brief Applies a rotation torque at the center of the body
    /// @param torque torque vector in world space
    void applyTorque(const glm::vec3 &torque) {
        totalTorque += torque;
    }

    /// @brief Applies a force relative to the body's coordinate space
    /// @param force force vector in local space
    void applyRelativeForce(const glm::vec3 &force) {
        // Convert [force] vector to relative coordinates before applying
        glm::vec3 relativeForce = glm::mat3{getRotation()} * force;
        applyForce(relativeForce);
    }

    /// @brief Applies a rotation torque relative to the body's coordinate space
    /// @param torque torque vector in local space
    void applyRelativeTorque(const glm::vec3 &torque) {
        // Convert [torque] vector to relative coordinates before applying
        glm::vec3 relativeTorque = glm::mat3{getRotation()} * torque;
        applyTorque(relativeTorque);
    }

    /// @brief Applies a force at a position. This will apply both a force and a torque on the body
    /// @param force force vector in world space
    /// @param pos position in world space
    void applyForceAtPosition(const glm::vec3 &force, const glm::vec3 &pos) {
        // Apply force at center of body normally
        applyForce(force);

        // Calculate torque based on position and center of mass
        glm::vec3 torque = glm::cross(pos - getPosition(), force);
        applyTorque(torque);
    }

    /// @brief Converts a point in world space to local space
    /// @param worldPoint point in world space
    /// @return point in local space
    glm::vec3 pointToLocalSpace(const glm::vec3 &worldPoint) const {
        glm::vec3 point = worldPoint - getPosition();
        return glm::inverse(glm::mat3{getRotation()}) * point;
    }

    /// @brief Converts a point in local space to world space
    /// @param worldPoint point in local space
    /// @return point in world space
    glm::vec3 pointToWorldSpace(const glm::vec3 &localPoint) const {
        glm::vec3 point = glm::mat3{getRotation()} * localPoint;
        return point + getPosition();
    }

    /// @brief Converts a vector in world space to local space
    /// @param worldVector vector in world space
    /// @return vector in local space
    glm::vec3 vectorToLocalSpace(const glm::vec3 &worldVector) const {
        return glm::inverse(glm::mat3{getRotation()}) * worldVector;
    }

    /// @brief Converts a vector in local space to world space
    /// @param localVector vector in local space
    /// @return vector in world space
    glm::vec3 vectorToWorldSpace(const glm::vec3 &localVector) const {
        return glm::mat3{getRotation()} * localVector;
    }

    /// @brief Gets object mass
    /// @return mass
    float getMass() const {
        return mass;
    }

    /// @brief Sets new linear drag
    /// @param drag linear drag
    void setLinearDrag(float drag) {
        linearDrag = std::max(0.0f, drag);
    }

    /// @brief Gets current linear drag
    /// @return linear drag
    float getLinearDrag() const {
        return linearDrag;
    }

    /// @brief Sets new angular drag
    /// @param drag angular drag
    void setAngularDrag(float drag) {
        angularDrag = std::max(0.0f, drag);
    }

    /// @brief Gets current angular drag
    /// @return angular drag
    float getAngularDrag() const {
        return angularDrag;
    }

private:
    /// -------------------------------------------------------------- ///
    ///                        MISCELLANEOUS                           ///
    /// -------------------------------------------------------------- ///

    /// @brief Entity for visualization of local x-axis
    std::shared_ptr<Entity> axisX;

    /// @brief Entity for visualization of local y-axis
    std::shared_ptr<Entity> axisY;

    /// @brief Entity for visualization of local z-axis
    std::shared_ptr<Entity> axisZ;

    /// -------------------------------------------------------------- ///
    ///                         CONSTANTS                              ///
    /// -------------------------------------------------------------- ///

    /// @brief Body mass
    float mass;

    /// @brief Inverse of body inertia tensor
    /// @link https://www.cs.cmu.edu/~baraff/sigcourse/notesd1.pdf
    glm::mat3 iBodyInv;

    /// -------------------------------------------------------------- ///
    ///                         PROPERTIES                             ///
    /// -------------------------------------------------------------- ///

    /// @brief How much the object's linear momentum slows down over time
    float linearDrag = 1.0f;

    /// @brief How much the object's angular momentum slows down over time
    float angularDrag = 0.5f;

    /// -------------------------------------------------------------- ///
    ///                        CURRENT STATE                           ///
    /// -------------------------------------------------------------- ///

    /// @brief Rotation quaternion
    glm::quat rot = glm::quat{1.0f, glm::vec3{0.0f}};

    /// @brief Linear momentum (change in position)
    glm::vec3 linearMomentum = glm::vec3{0.0f};

    /// @brief Angular momentum (change in rotation)
    glm::vec3 angularMomentum = glm::vec3{0.0f};

    /// -------------------------------------------------------------- ///
    ///                          AUXILIARY                             ///
    /// -------------------------------------------------------------- ///

    /// @brief Inverse of inertia tensor at current simulation step
    glm::mat3 iInv = glm::mat3{1.0f};

    /// @brief Cached rotation matrix at current simulation step
    glm::mat3 R = glm::mat3{1.0f};

    /// @brief Linear velocity at current simulation step
    glm::vec3 vel = glm::vec3{0.0f};

    /// @brief Angular velocity at current simulation step
    glm::vec3 omega = glm::vec3{0.0f};

    /// -------------------------------------------------------------- ///
    ///                   EXTERNAL INTERACTION                         ///
    /// -------------------------------------------------------------- ///

    /// @brief Total force accumulated (such as gravity and wind) at current simulation step
    glm::vec3 totalForce = glm::vec3{0.0f};

    /// @brief Total torque accumulated (such as collision) at current simulation step
    glm::vec3 totalTorque = glm::vec3{0.0f};
};

class App : public tmig::Window {
public:
    App();

    void setup() override;
    void update(float dt) override;
    void processInput(float dt) override;

    std::shared_ptr<tmig::Scene> rbScene;
    std::vector<std::shared_ptr<RigidBody>> rigidBodies;

    std::shared_ptr<tmig::Entity> sphere;
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