#ifndef PERSCAMERA_HPP
#define PERSCAMERA_HPP

#include "Camera.hpp"

class PersCamera : public Camera {
public:
	PersCamera(glm::vec3 pos, glm::vec3 up, glm::vec3 front);

	glm::mat4 getViewMatrix() override;
	void updateViewMatrix() override;
};

#endif