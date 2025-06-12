#ifndef ORTHOCAMERA_HPP
#define ORTHOCAMERA_HPP

#include "Camera.hpp"

class OrthoCamera : public Camera {
public:
    OrthoCamera(glm::vec3 pos, glm::vec3 up, glm::vec3 front);

    glm::mat4 getViewMatrix() override;
    void updateViewMatrix() override;
};

#endif