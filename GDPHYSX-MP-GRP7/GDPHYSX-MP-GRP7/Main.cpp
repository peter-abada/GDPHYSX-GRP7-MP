#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Model.hpp"
#include "Camera.hpp"
#include "OrthoCamera.hpp"
#include "PersCamera.hpp"
#include "Shader.hpp"

#include "Physics/Vector.hpp"
#include "Physics/PhysicsParticle.hpp"
#include "Physics/PhysicsWorld.hpp"
#include "RenderParticle.hpp"
#include "Physics/DragForceGenerator.hpp"
#include "Physics/ConstantForceGenerator.hpp"

int main(void)
{
    float width = 800.0f, height = 800.0f;

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(width, height, "GDPHYSX Group 7 - The Physics Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, 800, 800);

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
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * attributes.vertices.size(),
        &attributes.vertices[0],
        GL_STATIC_DRAW);

    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh_indices.size(), mesh_indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Camera orbit state (around origin)
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    float camDistance = 60.0f;
    float camYaw = glm::radians(90.0f);    // Start at +Z
    float camPitch = glm::radians(20.0f);  // Slightly above

    // Camera setup
    OrthoCamera orthoCamera(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    PersCamera persCamera(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    Camera* activeCamera = &orthoCamera;

    // Projection matrices
    glm::mat4 orthoProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
    glm::mat4 perspProjection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 200.0f);

    glm::mat4* activeProjection = &orthoProjection;

    // Camera update lambda
    auto updateCamera = [&]() {
        float x = camDistance * cos(camPitch) * cos(camYaw);
        float y = camDistance * sin(camPitch);
        float z = camDistance * cos(camPitch) * sin(camYaw);

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
    updateCamera();

    Model model(glm::vec3(0.0f, 0.0f, 0.0f), shader.getProg(), VAO, mesh_indices);

    Physics::PhysicsWorld world;
    std::list<RenderParticle*> renderParticles;
    int sparkNumber = 0;
    std::cout << "Enter particle amount (integer): ";
    std::cin >> sparkNumber;

    srand(static_cast<unsigned int>(time(NULL)));

    for (int i = 0; i < sparkNumber; i++) {
        Physics::PhysicsParticle* particle = new Physics::PhysicsParticle();
        particle->position = Physics::Vector(0, -40, 0); // Firework origin

        float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.1415926f;
        float phi = (static_cast<float>(rand()) / RAND_MAX) * (3.1415926f / 12.0f); // [0, pi/12] 15 deg
        float speed = 30.0f + static_cast<float>(rand()) / RAND_MAX * 20.0f;

        float vx = speed * sin(phi) * cos(theta);
        float vy = speed * cos(phi);
        float vz = speed * sin(phi) * sin(theta);

        particle->velocity = Physics::Vector(vx, vy, vz);

        particle->mass = 0.5f;
        particle->lifespan = 1.0f + static_cast<float>(rand()) / RAND_MAX * 9.0f;
        world.addParticle(particle);

        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;

        float scale = 0.2f + static_cast<float>(rand()) / RAND_MAX * 0.8f;
        Model* particleModel = new Model(model);
        glm::vec3 scaleVec(scale, scale, scale);
        particleModel->setScale(scaleVec);

        RenderParticle* renderParticle = new RenderParticle(particle, particleModel, Physics::Vector(r, g, b));
        renderParticles.push_back(renderParticle);
    }

    typedef std::chrono::high_resolution_clock clock;
    clock::time_point curr_time = clock::now();
    clock::time_point prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    bool paused = false;
    bool spacePressedLastFrame = false;
    bool onePressedLastFrame = false;
    bool twoPressedLastFrame = false;

    std::cout << renderParticles.size() << "\n";

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        float orbitSpeed = 1.5f * (float)timestep.count() / 1e9f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camYaw += orbitSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camYaw -= orbitSpeed;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camPitch += orbitSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camPitch -= orbitSpeed;
        float pitchLimit = glm::radians(89.0f);
        if (camPitch > pitchLimit) camPitch = pitchLimit;
        if (camPitch < -pitchLimit) camPitch = -pitchLimit;

        updateCamera();

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

        // Play/pause toggle (with debounce)
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spacePressed && !spacePressedLastFrame) paused = !paused;
        spacePressedLastFrame = spacePressed;

        curr_time = clock::now();
        std::chrono::nanoseconds dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;

        curr_ns += dur;
        if (curr_ns >= timestep) {
            std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            curr_ns -= curr_ns;

            if (!paused) {
                world.update((float)ms.count() / 1000.0f);
            }

            std::list<RenderParticle*>::iterator it = renderParticles.begin();
            while (it != renderParticles.end()) {
                Physics::PhysicsParticle* p = (*it)->particle;
                if (!paused) p->lifespan -= (float)ms.count() / 1000.0f;
                if (p->lifespan <= 0.0f) {
                    delete (*it)->renderObj;
                    delete* it;
                    it = renderParticles.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = activeCamera->getViewMatrix();
        glm::mat4 projView = (*activeProjection) * viewMatrix;
        glUniformMatrix4fv(glGetUniformLocation(shader.getProg(), "projection"), 1, GL_FALSE, glm::value_ptr(projView));

        for (RenderParticle* rp : renderParticles) {
            glUniform3f(glGetUniformLocation(shader.getProg(), "color"), rp->color.x, rp->color.y, rp->color.z);

            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), rp->renderObj->getPosition());
            modelMatrix = glm::scale(modelMatrix, rp->renderObj->getScale());
            glUniformMatrix4fv(glGetUniformLocation(shader.getProg(), "transform"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

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
