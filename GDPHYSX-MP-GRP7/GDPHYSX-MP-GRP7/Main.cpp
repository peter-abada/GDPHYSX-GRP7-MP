#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);
#include <iostream>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Model.hpp"
#include "Camera.hpp"
#include "OrthoCamera.hpp"
#include "Shader.hpp"

#include "Physics/Vector.hpp"
#include "Physics/PhysicsParticle.hpp"
#include "Physics/PhysicsWorld.hpp"
#include "RenderParticle.hpp"
#include "Physics/DragForceGenerator.hpp"
#include "Physics/ConstantForceGenerator.hpp"

float pAccel = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        pAccel += 0.1;
    }
}

int main(void)
{
    float width = 600.0f, height = 600.0f;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "PC01 Peter Abada", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, 600, 600);

    glfwSetKeyCallback(window, key_callback);

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

    GLfloat vertices[]{
        -0.5f, -0.5f, 0,
        0, 0.5f, 0,
        0.5f, -0.5f, 0
    };

    GLuint indices[] = {
        0,1,2
    };

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

    //Projection matrix
    glm::mat4 projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);

    Model model(glm::vec3(0.0f, 0.0f, 0.0f), shader.getProg(), VAO, mesh_indices);
    
    OrthoCamera camera(glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    Physics::Vector sample(2, 3, -4);
    Physics::Vector sample2(-3, -2, 2);
    Physics::Vector sum = sample & sample2;
    /* Loop until the user closes the window */

    Physics::PhysicsWorld world;
    std::list<RenderParticle*> renderParticles;

    srand(time(NULL));

    Physics::PhysicsParticle p1 = Physics::PhysicsParticle();
    p1.position = Physics::Vector(-50, 40, 0);
    p1.mass = 1;
    //p1.addForce(Physics::Vector(rand() % 600 + 500, 0, 0));
    world.addParticle(&p1);
    RenderParticle rp1 = RenderParticle(&p1, &model, Physics::Vector(0.4, 0, 0));
    rp1.color = Physics::Vector(1, 0, 0);
    renderParticles.push_back(&rp1);
    Physics::ConstantForceGenerator constforce1 = Physics::ConstantForceGenerator(Physics::Vector(1, 0, 0));;
    srand(time(0));
    world.forceRegistry.add(&p1, &constforce1);

    Physics::PhysicsParticle p2 = Physics::PhysicsParticle();
    p2.position = Physics::Vector(-50, 20, 0);
    p2.mass = 1;
    p2.addForce(Physics::Vector(rand() % 600 + 500, 0, 0));
    world.addParticle(&p2);
    RenderParticle rp2 = RenderParticle(&p2, &model, Physics::Vector(0.6f, 0, 0));
    rp2.color = Physics::Vector(0, 1, 0);
    renderParticles.push_back(&rp2);
    Physics::ConstantForceGenerator constforce2 = Physics::ConstantForceGenerator(Physics::Vector(1, 0, 0));;
    srand(time(0));
    world.forceRegistry.add(&p2, &constforce2);

    Physics::PhysicsParticle p3 = Physics::PhysicsParticle();
    p3.position = Physics::Vector(-50, 0, 0);
    p3.mass = 1;
    p3.addForce(Physics::Vector(rand() % 600 + 500, 0, 0));
    world.addParticle(&p3);
    RenderParticle rp3 = RenderParticle(&p3, &model, Physics::Vector(0, 0.5f, 0));
    rp3.color = Physics::Vector(0, 0, 1);
    renderParticles.push_back(&rp3);
    Physics::ConstantForceGenerator constforce3 = Physics::ConstantForceGenerator(Physics::Vector(1, 0, 0));;
    srand(time(0));
    world.forceRegistry.add(&p3, &constforce3);

    Physics::PhysicsParticle p4 = Physics::PhysicsParticle();
    p4.position = Physics::Vector(-50, -20, 0);
    p4.mass = 1;
    p4.addForce(Physics::Vector(700, 0, 0));
    world.addParticle(&p4);
    RenderParticle rp4 = RenderParticle(&p4, &model, Physics::Vector(0, 0.5f, 0));
    rp4.color = Physics::Vector(0.5, 0.5, 0.5);
    renderParticles.push_back(&rp4);

    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    glm::vec3 direction = glm::normalize(camera.getFront());
    float distance = glm::length(camera.getPosition() - model.getPosition());
    glm::vec3 newPosition = model.getPosition() - direction * distance;
    camera.setPosition(newPosition);
    glm::mat4 viewMatrix = camera.getViewMatrix();

    srand(time(NULL));
    //float p1f = rand() % 8 + 1;
    float p2f = rand() % 8 + 1;
    float p3f = rand() % 8 + 1;

    int p1time = 0;
    int p2time = 0;
    int p3time = 0;
    int p4time = 0;

    while (!glfwWindowShouldClose(window) && (p1.position.x < 50 || p2.position.x < 50 || p3.position.x < 50 || p4.position.x < 50)) {

        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (curr_time - prev_time);
        prev_time = curr_time;

        curr_ns += dur;
        if (curr_ns >= timestep) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (curr_ns);
            //std::cout << "MS: " << (float)ms.count() << "\n";
            curr_ns -= curr_ns;

            //Update
            //std::cout << "Update" << "\n";
            world.update((float)ms.count() / 1000);
            pAccel == 0;
            if (p1.position.x < 50)
                p1time++;
            if (p2.position.x < 50)
                p2time++;
            if (p3.position.x < 50)
                p3time++;
            if (p4.position.x < 50)
                p4time++;
        }
        //std::cout << "Normal" << "\n";
        p4.addForce(Physics::Vector(pAccel, 0, 0));
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        //Camera position
        
        //std::cout << pAccel << "\n";

        unsigned int projLoc = glGetUniformLocation(shader.getProg(), "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        /*unsigned int viewLoc = glGetUniformLocation(shader.getProg(), "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));*/
        for (std::list<RenderParticle*>::iterator i = renderParticles.begin(); i != renderParticles.end(); i++) {
            (*i)->draw();
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    std::cout << "P1 Time: " << p1time / 64 << "\n";
    std::cout << "P2 Time: " << p2time / 64 << "\n";
    std::cout << "P3 Time: " << p3time / 64 << "\n";
    std::cout << "P4 Time: " << p4time / 64 << "\n";

    glfwTerminate();
    return 0;
}