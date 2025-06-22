#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Representa a câmera na cena, com posição, direção e vetor up
 */
class Camera {
public:
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;

    Camera(const glm::vec3& position,
           const glm::vec3& frontVec,
           const glm::vec3& upVec);

    glm::mat4 getViewMatrix() const;
};