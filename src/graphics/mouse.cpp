#include "mouse.h"

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Mouse::Mouse(float yaw,
             float pitch,
             float lastX,
             float lastY,
             bool  firstMouse,
             float fov)
    : yaw(yaw)
    , pitch(pitch)
    , lastX(lastX)
    , lastY(lastY)
    , firstMouse(firstMouse)
    , fov(fov)
{}

Mouse::~Mouse() = default;