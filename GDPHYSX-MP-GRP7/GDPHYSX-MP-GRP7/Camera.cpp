#include "Camera.hpp"

Camera::Camera(glm::vec3 pos, glm::vec3 up, glm::vec3 front)
    : position(pos), up(up), front(front) {
}

void Camera::setPosition(const glm::vec3& pos) {
    position = pos;
    updateViewMatrix();
}

void Camera::setFront(const glm::vec3& fr) {
    front = fr;
    updateViewMatrix();
}

void Camera::setUp(const glm::vec3& u) {
    up = u;
    updateViewMatrix();
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::vec3 Camera::getFront() const {
    return front;
}

glm::vec3 Camera::getUp() const {
    return up;
}