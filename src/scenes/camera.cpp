#include "camera.h"

Camera::Camera(const glm::vec3& position,
               const glm::vec3& frontVec,
               const glm::vec3& upVec)
    : pos(position)
    , front(frontVec)
    , up(upVec)
{}

// Gera a matriz de view usando glm::lookAt
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(pos, pos + front, up);
}