#include "scene.h"
#include "camera.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

Scene::Scene(const Shader& shader)
    : mCamera(
        glm::vec3(0.0f, 1.1f, 8.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
      ),
      mShader(shader)  // depois constrói o shader
{
    // corpo vazio!
}

Scene::~Scene() = default;

void Scene::prepare() {
    // TODO
}

void Scene::draw() {
    // TODO
}
