#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Model {
private:
	

public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    GLuint shaderProg;
    GLuint VAO;
    std::vector<GLuint>& mesh_indices;

	Model(glm::vec3 pos, GLuint prog, GLuint vao, std::vector<GLuint>& indices);

    void draw();
    glm::vec3 getPosition();
    void setPosition(glm::vec3& pos);
    glm::vec3 getRotation();
    void setRotation(glm::vec3& rot);
    glm::vec3 getScale();
    void setScale(glm::vec3& scl);

};


#endif