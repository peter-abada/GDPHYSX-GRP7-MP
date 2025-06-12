#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
protected:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 front;
    glm::mat4 viewMatrix;

public:
    Camera(glm::vec3 pos, glm::vec3 up, glm::vec3 front);

    virtual glm::mat4 getViewMatrix() = 0;
    virtual void updateViewMatrix() = 0;

    void setPosition(const glm::vec3& pos);
    void setFront(const glm::vec3& fr);
    void setUp(const glm::vec3& u);

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;
};

#endif