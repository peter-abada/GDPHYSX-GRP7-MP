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
#include "Shader.hpp"

#include "Krazy/Vector.hpp"
#include "Krazy/PhysicsParticle.hpp"
#include "Krazy/PhysicsWorld.hpp"
#include "RenderParticle.hpp"
#include "Krazy/ParticleCable.hpp"

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

    GLFWwindow* window = glfwCreateWindow(width, height, "GDPHYSX Group 7 - Newton's Cradle (Cable)", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, (int)width, (int)height);

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

    glm::mat4 projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
    OrthoCamera camera(glm::vec3(0.0f, 0.0f, 60.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

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
    std::cout << "[INFO] Center-to-center distance is 2 * radius + gap = " << centerToCenter << std::endl;

    const int numBalls = 5;
    float startX = -centerToCenter * 2; // Center the 3rd ball at x=0
    float anchorY = 30.0f;              // Anchor height above origin

    for (int i = 0; i < numBalls; ++i) {
        float x = startX + i * centerToCenter;
        float y = anchorY; // Start at anchor point (top of cable)

        Krazy::PhysicsParticle* ball = new Krazy::PhysicsParticle();
        ball->position = Krazy::Vector(x, y, 0); // At anchor
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

        // Use ParticleCable for the cable constraint
        Krazy::ParticleCable* cable = new Krazy::ParticleCable(ball, Krazy::Vector(x, anchorY, 0), cableLength, 0.0f);
        world.links.push_back(cable);

        std::cout << "Ball " << i << " initial pos: (" << x << ", " << y << ", " << 0 << ")\n";
    }

    world.setGravity(Krazy::GravityForceGenerator(Krazy::Vector(0, -gravityStrength, 0)));
    bool gravityEnabled = false;
    bool forceApplied = false;
    bool paused = false;
    bool spacePressed = false;

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

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            spacePressed = true;
        }

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.getProg());
        unsigned int projLoc = glGetUniformLocation(shader.getProg(), "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

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

            // Set transform for the sphere
            GLint transformLoc = glGetUniformLocation(shader.getProg(), "transform");
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, rp->renderObj->getScale());
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &model[0][0]);

            rp->draw();
        }

        static int debugFrame = 0;
        if (++debugFrame >= 30) {
            std::cout << "Ball positions: ";
            for (size_t i = 0; i < particles.size(); ++i) {
                const auto& p = particles[i]->position;
                std::cout << "[" << i << "](" << p.x << ", " << p.y << ", " << p.z << ") ";
            }
            std::cout << std::endl;
            debugFrame = 0;
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
