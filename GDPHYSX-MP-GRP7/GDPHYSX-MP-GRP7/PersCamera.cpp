#include "PersCamera.hpp"

PersCamera::PersCamera(glm::vec3 pos, glm::vec3 up, glm::vec3 front) : Camera(pos, up, front) {
	updateViewMatrix();
}

glm::mat4 PersCamera::getViewMatrix() {
	return viewMatrix;
}

void PersCamera::updateViewMatrix() {
	viewMatrix = glm::lookAt(position, position + front, up);
}