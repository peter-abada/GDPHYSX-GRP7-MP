#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);
#include <iostream>
#include <vector>
#include <cmath>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Model.hpp"
#include "Camera.hpp"
#include "OrthoCamera.hpp"
#include "PersCamera.hpp"
#include "Shader.hpp"

#include "Krazy/Vector.hpp"
#include "Krazy/PhysicsParticle.hpp"
#include "Krazy/PhysicsWorld.hpp"
#include "RenderParticle.hpp"
#include "Krazy/ParticleCable.hpp"

/*

    Resolves collisions between particles in the simulation

    PARAMETERS:
    vector<PhysicsParticle*>& particles - The list of all particles in the simulation

*/
void resolveBallCollisions(std::vector<Krazy::PhysicsParticle*>& particles) {
    for (size_t i = 0; i < particles.size(); ++i) {
        for (size_t j = i + 1; j < particles.size(); ++j) {
            Krazy::PhysicsParticle* a = particles[i];
            Krazy::PhysicsParticle* b = particles[j];

            if (a->mass <= 0 || b->mass <= 0) continue;

            Krazy::Vector delta = b->position - a->position;
            float dist = delta.Magnitude();
            float minDist = a->radius + b->radius;

            if (dist < minDist && dist > 0.0001f) {
                float penetration = minDist - dist;
                Krazy::Vector normal = delta.Normalize();

                float invMassA = 1.0f / a->mass;
                float invMassB = 1.0f / b->mass;
                float totalInvMass = invMassA + invMassB;

                a->position -= normal * (penetration * (invMassA / totalInvMass));
                b->position += normal * (penetration * (invMassB / totalInvMass));

                Krazy::Vector relVel = b->velocity - a->velocity;
                float sepVel = relVel.x * normal.x + relVel.y * normal.y + relVel.z * normal.z;

                if (sepVel < 0) {
                    float restitution = std::min(a->restitution, b->restitution);
                    float newSepVel = -sepVel * restitution;
                    float deltaVel = newSepVel - sepVel;
                    float impulse = deltaVel / totalInvMass;
                    Krazy::Vector impulsePerInvMass = normal * impulse;
                    a->velocity -= impulsePerInvMass * invMassA;
                    b->velocity += impulsePerInvMass * invMassB;
                }
            }
        }
    }
}

/*
    Boolean function for checking if all particles are at rest (not moving)

    PARAMETERS:
    vector<PhysicsParticle*> particles - The list of all particles in the simulation
    float anchorY - Y position of the anchor points the cables are hooked to
    float cableLength - Length of the cables holding the particles
*/
bool allBallsAtRest(const std::vector<Krazy::PhysicsParticle*>& particles, float anchorY, float cableLength, float epsilon = 0.1f, float velEpsilon = 0.1f) {
    for (const auto* p : particles) {
        float dist = (p->position - Krazy::Vector(p->position.x, anchorY, 0)).Magnitude();
        if (std::abs(dist - cableLength) > epsilon) return false;
        if (p->velocity.Magnitude() > velEpsilon) return false;
    }
    return true;
}

int main(void)
{
    float width = 800.0f, height = 800.0f;

    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "GRP7 - Krazy Engine: Newton's Cradle", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, (int)width, (int)height);

    glEnable(GL_DEPTH_TEST);

    Shader shader("Shaders/Shader.vert", "Shaders/Shader.frag");
    glLinkProgram(shader.getProg());

    // Load sphere mesh
    std::string path = "3D/sphere.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;
    tinyobj::attrib_t attributes;
    bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());

    std::vector<GLuint> mesh_indices;
    if (success && !shapes.empty()) {
        for (size_t i = 0; i < shapes[0].mesh.indices.size(); i++) {
            mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
        }
    }
    else {
        mesh_indices = { 0, 1, 2 };
        attributes.vertices = { -0.5f, -0.5f, 0, 0, 0.5f, 0, 0.5f, -0.5f, 0 };
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * attributes.vertices.size(), &attributes.vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::vec3 target(0.0f, 0.0f, 0.0f);

    float orthoHalfWidth = width / 2.0f;  
    float orthoHalfHeight = height / 2.0f; 
   
    float orthoNear = 0.1f, orthoFar = 2000.0f;
    float perspFov = glm::radians(45.0f);
    float camDistance = 600.0f; 
    float camDistancePersp = 1000.0f; 
    float camYaw = glm::radians(90.0f);
    float camPitch = glm::radians(20.0f);

    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, camDistance);
    glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camFront = glm::normalize(camTarget - camPos);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    OrthoCamera orthoCamera(camPos, camUp, camFront);
    PersCamera persCamera(camPos, camUp, camFront);
    Camera* activeCamera = &orthoCamera;

    glm::mat4 orthoProjection = glm::ortho(-orthoHalfWidth, orthoHalfWidth, -orthoHalfHeight, orthoHalfHeight, orthoNear, orthoFar);
    glm::mat4 perspProjection = glm::perspective(perspFov, width / height, 0.1f, 2000.0f);
    glm::mat4* activeProjection = &orthoProjection;

    //Lambda function for updating the camera orientation
    auto updateCamera = [&](float effectiveCamDistance) {
        float x = effectiveCamDistance * cos(camPitch) * cos(camYaw);
        float y = effectiveCamDistance * sin(camPitch);
        float z = effectiveCamDistance * cos(camPitch) * sin(camYaw);

        glm::vec3 camPos = target + glm::vec3(x, y, z);
        glm::vec3 front = glm::normalize(target - camPos);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        orthoCamera.setPosition(camPos);
        orthoCamera.setFront(front);
        orthoCamera.setUp(up);
        orthoCamera.updateViewMatrix();

        persCamera.setPosition(camPos);
        persCamera.setFront(front);
        persCamera.setUp(up);
        persCamera.updateViewMatrix();
        };

    updateCamera(camDistance);

    //User input for simulation parameters
    Krazy::PhysicsWorld world;
    std::vector<RenderParticle*> renderParticles;
    std::vector<Krazy::PhysicsParticle*> particles;
    float cableLength, particleGap, particleRadius, gravityStrength, forceX, forceY, forceZ;
    std::cout << "Cable Length: ";
    std::cin >> cableLength;
    std::cout << "Particle Gap: ";
    std::cin >> particleGap;
    std::cout << "Particle Radius: ";
    std::cin >> particleRadius;
    std::cout << "Gravity Strength: ";
    std::cin >> gravityStrength;
    std::cout << "Apply Force\nX: ";
    std::cin >> forceX;
    std::cout << "Y: ";
    std::cin >> forceY;
    std::cout << "Z: ";
    std::cin >> forceZ;

    float centerToCenter = 2.0f * particleRadius + particleGap;

    const int numBalls = 5;
    float startX = -centerToCenter * 2;
    float anchorY = 30.0f;

    //Generate particles for the simulation
    for (int i = 0; i < numBalls; ++i) {
        float x = startX + i * centerToCenter;
        float y = anchorY;

        Krazy::PhysicsParticle* ball = new Krazy::PhysicsParticle();
        ball->position = Krazy::Vector(x, y, 0);
        ball->mass = 50.0f;
        ball->velocity = Krazy::Vector(0, 0, 0);
        ball->damping = 0.98f;
        ball->radius = particleRadius;
        ball->restitution = 0.9f;
        ball->lifespan = 1000.0f;
        world.particles.push_back(ball);
        particles.push_back(ball);

        Model* ballModel = new Model(glm::vec3(x, y, 0), shader.getProg(), VAO, mesh_indices);
        glm::vec3 scaleVec(particleRadius, particleRadius, particleRadius);
        ballModel->setScale(scaleVec);
        RenderParticle* ballRender = new RenderParticle(ball, ballModel, Krazy::Vector(0.7f, 0.7f, 1.0f));
        renderParticles.push_back(ballRender);

        Krazy::ParticleCable* cable = new Krazy::ParticleCable(ball, Krazy::Vector(x, anchorY, 0), cableLength, 0.0f);
        world.links.push_back(cable);

        std::cout << "Ball " << i << " initial pos: (" << x << ", " << y << ", " << 0 << ")\n";
    }

    world.setGravity(Krazy::GravityForceGenerator(Krazy::Vector(0, -gravityStrength, 0)));
    bool gravityEnabled = false;
    bool forceApplied = false;
    bool paused = false;
    bool spacePressed = false;
    bool spacePressedLastFrame = false;
    bool onePressedLastFrame = false;
    bool twoPressedLastFrame = false;

    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    while (!glfwWindowShouldClose(window)) {
        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;
        curr_ns += dur;

        glfwPollEvents();

        float orbitSpeed = 1.5f * (float)timestep.count() / 1e9f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camYaw += orbitSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camYaw -= orbitSpeed;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPitch += orbitSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPitch -= orbitSpeed;
        float pitchLimit = glm::radians(89.0f);
        if (camPitch > pitchLimit) camPitch = pitchLimit;
        if (camPitch < -pitchLimit) camPitch = -pitchLimit;

        // Camera mode switching (1 = ortho, 2 = perspective)
        bool onePressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
        bool twoPressed = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
        if (onePressed && !onePressedLastFrame) {
            activeCamera = &orthoCamera;
            activeProjection = &orthoProjection;
        }
        if (twoPressed && !twoPressedLastFrame) {
            activeCamera = &persCamera;
            activeProjection = &perspProjection;
        }
        onePressedLastFrame = onePressed;
        twoPressedLastFrame = twoPressed;

        float effectiveCamDistance = (activeCamera == &persCamera) ? camDistancePersp : camDistance;
        updateCamera(effectiveCamDistance);

        bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spaceNow && !spacePressedLastFrame) spacePressed = true;
        spacePressedLastFrame = spaceNow;

        if (!forceApplied && allBallsAtRest(particles, anchorY, cableLength) && spacePressed) {
            particles[0]->addForce(Krazy::Vector(forceX, forceY, forceZ));
            forceApplied = true;
            gravityEnabled = true;
            world.setGravity(Krazy::GravityForceGenerator(Krazy::Vector(0, -gravityStrength, 0)));
            std::cout << "[DEBUG] All balls at rest and space pressed. Force applied to particle 0: (" << forceX << ", " << forceY << ", " << forceZ << ")\n";
        }

        if (curr_ns >= timestep) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            curr_ns -= curr_ns;
            if (!paused) {
                world.update((float)ms.count() / 1000.0f);
                resolveBallCollisions(particles);
            }
        }

        if (activeCamera == &orthoCamera) {
            float scale = std::sqrt(2.0f);
            orthoProjection = glm::ortho(
                -orthoHalfWidth * scale, orthoHalfWidth * scale,
                -orthoHalfHeight * scale, orthoHalfHeight * scale,
                orthoNear, orthoFar
            );
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = activeCamera->getViewMatrix();
        glm::mat4 projView = (*activeProjection) * viewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(shader.getProg(), "projection"), 1, GL_FALSE, glm::value_ptr(projView));

        // Draw cables as lines (from anchor to particle center)
        for (int i = 0; i < numBalls; ++i) {
            glm::vec3 anchor = glm::vec3(startX + i * centerToCenter, anchorY, 0.0f);
            glm::vec3 pos = glm::vec3(particles[i]->position.x, particles[i]->position.y, particles[i]->position.z);

            float lineVertices[] = {
                anchor.x, anchor.y, anchor.z,
                pos.x, pos.y, pos.z
            };

            GLuint lineVAO, lineVBO;
            glGenVertexArrays(1, &lineVAO);
            glGenBuffers(1, &lineVBO);
            glBindVertexArray(lineVAO);
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            GLint colorLoc = glGetUniformLocation(shader.getProg(), "color");
            glUniform3f(colorLoc, 0.1f, 0.1f, 0.1f);

            GLint transformLoc = glGetUniformLocation(shader.getProg(), "transform");
            glm::mat4 identity = glm::mat4(1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &identity[0][0]);

            glDrawArrays(GL_LINES, 0, 2);

            glDeleteBuffers(1, &lineVBO);
            glDeleteVertexArrays(1, &lineVAO);
        }

        // Draw particles
        for (auto* rp : renderParticles) {
            glm::vec3 pos(rp->particle->position.x, rp->particle->position.y, rp->particle->position.z);
            rp->renderObj->setPosition(pos);

            GLint colorLoc = glGetUniformLocation(shader.getProg(), "color");
            glUniform3f(colorLoc, rp->color.x, rp->color.y, rp->color.z);

            GLint transformLoc = glGetUniformLocation(shader.getProg(), "transform");
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, rp->renderObj->getScale());
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &model[0][0]);

            rp->draw();
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
