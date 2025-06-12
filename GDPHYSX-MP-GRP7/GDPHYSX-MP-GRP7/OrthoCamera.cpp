#include "OrthoCamera.hpp"

OrthoCamera::OrthoCamera(glm::vec3 pos, glm::vec3 up, glm::vec3 front)
    : Camera(pos, up, front) {
    updateViewMatrix();
}

glm::mat4 OrthoCamera::getViewMatrix() {
    return viewMatrix;
}

void OrthoCamera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + front, up);
}