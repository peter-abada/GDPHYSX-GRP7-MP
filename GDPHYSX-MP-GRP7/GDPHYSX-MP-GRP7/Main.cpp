#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

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

            // If balls overlap, resolve penetration and velocity
            if (dist < minDist && dist > 0.0001f) {
                float penetration = minDist - dist;
                Krazy::Vector normal = delta.Normalize();

                float invMassA = 1.0f / a->mass;
                float invMassB = 1.0f / b->mass;
                float totalInvMass = invMassA + invMassB;

                // Move balls apart
                a->position -= normal * (penetration * (invMassA / totalInvMass));
                b->position += normal * (penetration * (invMassB / totalInvMass));

                Krazy::Vector relVel = b->velocity - a->velocity;
                float sepVel = relVel.x * normal.x + relVel.y * normal.y + relVel.z * normal.z;

                // If balls are moving toward each other, apply impulse
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

// Finds the index of the ball with the highest y position
int getTopBallIndex(const std::vector<Krazy::PhysicsParticle*>& particles) {
    int topIdx = 0;
    float maxY = particles[0]->position.y;
    for (size_t i = 1; i < particles.size(); ++i) {
        if (particles[i]->position.y > maxY) {
            maxY = particles[i]->position.y;
            topIdx = (int)i;
        }
    }
    return topIdx;
}

int main(void)
{
    float width = 800.0f, height = 800.0f;

    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "PC02 - Valdez", NULL, NULL);
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

    // Camera setup
    float perspFov = glm::radians(45.0f);
    float camDistancePersp = 100.0f;
    float camYaw = glm::radians(90.0f);
    float camPitch = glm::radians(20.0f);

    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, camDistancePersp);
    glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camFront = glm::normalize(camTarget - camPos);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    PersCamera persCamera(camPos, camUp, camFront);
    Camera* activeCamera = &persCamera;

    glm::mat4 perspProjection = glm::perspective(perspFov, width / height, 0.1f, 2000.0f);
    glm::mat4* activeProjection = &perspProjection;

    auto updateCamera = [&](float effectiveCamDistance) {
        float x = effectiveCamDistance * cos(camPitch) * cos(camYaw);
        float y = effectiveCamDistance * sin(camPitch);
        float z = effectiveCamDistance * cos(camPitch) * sin(camYaw);

        glm::vec3 camPos = target + glm::vec3(x, y, z);
        glm::vec3 front = glm::normalize(target - camPos);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        persCamera.setPosition(camPos);
        persCamera.setFront(front);
        persCamera.setUp(up);
        persCamera.updateViewMatrix();
        };

    updateCamera(camDistancePersp);

    Krazy::PhysicsWorld world;
    std::vector<RenderParticle*> renderParticles;
    std::vector<Krazy::PhysicsParticle*> particles;

    // Simulation parameters
    const int numBalls = 5;
    float cableLength = 30.0f;
    float particleRadius = 5.0f;
    float ballMass = 50.0f; 
    float circleRadius = cableLength;

    // Ball color names, prizes, and RGB colors
    std::vector<std::string> ballNames = {
        "Blue",
        "Red",
        "Green",
        "Purple",
        "Orange"
    };
    std::vector<std::string> ballPrizes = {
        "Ice Card",
        "Fire Card",
        "30 Gold",
        "Shop Ticket",
        "Old Boot"
    };
    std::vector<Krazy::Vector> ballColors = {
        Krazy::Vector(0.2f, 0.4f, 1.0f),   // Blue
        Krazy::Vector(1.0f, 0.2f, 0.2f),   // Red
        Krazy::Vector(0.2f, 1.0f, 0.3f),   // Green
        Krazy::Vector(0.7f, 0.2f, 0.9f),   // Purple
        Krazy::Vector(1.0f, 0.6f, 0.1f)    // Orange
    };

    std::cout << "List of colors and prizes:\n";
    for (size_t i = 0; i < ballNames.size(); ++i) {
        std::cout << ballNames[i] << " - Color: ("
            << ballColors[i].x << ", "
            << ballColors[i].y << ", "
            << ballColors[i].z << ")"
            << " - Prize: " << ballPrizes[i] << "\n";
    }

    float forceX, forceY;
    std::cout << "Enter force to apply\nX: ";
    std::cin >> forceX;
    std::cout << "Y: ";
    std::cin >> forceY;
    float forceZ = 0.0f;

    glm::vec3 anchor(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < numBalls; ++i) {
        // Arrange balls in a circle
        float angle = glm::radians(90.0f - i * 360.0f / numBalls);
        float x = circleRadius * cos(angle);
        float y = circleRadius * sin(angle);
        float z = 0.0f;

        // Create physics particle for each ball
        Krazy::PhysicsParticle* ball = new Krazy::PhysicsParticle();
        ball->position = Krazy::Vector(x, y, z);
        ball->mass = 50.0f;
        ball->velocity = Krazy::Vector(0, 0, 0);
        ball->damping = 0.98f;
        ball->radius = particleRadius;
        ball->restitution = 0.9f;
        ball->lifespan = 1000.0f;
        world.particles.push_back(ball);
        particles.push_back(ball);

        Model* ballModel = new Model(glm::vec3(x, y, z), shader.getProg(), VAO, mesh_indices);
        glm::vec3 scaleVec(particleRadius, particleRadius, particleRadius);
        ballModel->setScale(scaleVec);
        RenderParticle* ballRender = new RenderParticle(ball, ballModel, ballColors[i]);
        renderParticles.push_back(ballRender);
        Krazy::ParticleCable* cable = new Krazy::ParticleCable(ball, Krazy::Vector(anchor.x, anchor.y, anchor.z), cableLength, 0.0f);
        world.links.push_back(cable);
    }

    // Apply inputted force to top particle
    particles[0]->addForce(Krazy::Vector(forceX, forceY, forceZ));

    bool paused = false;
    bool frozen = false;
    bool waitingForEnter = false;
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

        updateCamera(camDistancePersp);

        // Freeze simulation on SPACE key, quit on ENTER key
        if (!frozen && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            for (auto* p : particles) {
                p->velocity = Krazy::Vector(0, 0, 0);
            }
            frozen = true;
            waitingForEnter = true;
            std::cout << "\nPress ENTER to exit\n";
        }

        if (curr_ns >= timestep) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            curr_ns -= curr_ns;
            if (!paused && !frozen) {
                world.update((float)ms.count() / 1000.0f);
                resolveBallCollisions(particles);
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = activeCamera->getViewMatrix();
        glm::mat4 projView = (*activeProjection) * viewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(shader.getProg(), "projection"), 1, GL_FALSE, glm::value_ptr(projView));

        // Render lines for cables
        for (int i = 0; i < numBalls; ++i) {
            glm::vec3 anchorPos = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 pos = glm::vec3(particles[i]->position.x, particles[i]->position.y, particles[i]->position.z);

            float lineVertices[] = {
                anchorPos.x, anchorPos.y, anchorPos.z,
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

        // Render particles
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
        if (waitingForEnter) {
            if (std::cin.rdbuf()->in_avail() > 0) {
                char c = std::cin.get();
                if (c == '\n') {
                    break;
                }
            }
        }

        glfwSwapBuffers(window);
    }

    // Print highest particle and corresponding prize
    int topIdx = getTopBallIndex(particles);
    std::cout << "\nTop most ball is " << ballNames[topIdx] << ".\n";
    std::cout << "You win: " << ballPrizes[topIdx] << "!\n";

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
