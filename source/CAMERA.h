//
// Created by Atlas on 8/11/2025.
//
#ifndef CAMERA_H
#define CAMERA_H
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

struct Camera {
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f), cameraFront, cameraUp = glm::vec3(0.0f, 1.0f, 0.0f), direction;
    float yaw = -90, pitch;
    void updateDirection() {
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }
    glm::mat4 getView() const {return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);}
};
#endif //CAMERA_H