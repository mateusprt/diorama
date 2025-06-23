#pragma once

#include <glm/glm.hpp>
#include <string>

using namespace std;

/**
 * @brief Propriedades do material de um objeto para iluminação Phong
 */
class Material {
public:
    glm::vec3 Ka;       // cor ambiente
    glm::vec3 Kd;       // cor difusa
    glm::vec3 Ks;       // cor especular
    float     Ns;       // shininess (exponente especular)
    string map_Kd; // caminho da textura

    // construtor padrão
    Material() = default;

    // construtor completo
    Material(glm::vec3 Ka,
             glm::vec3 Kd,
             glm::vec3 Ks,
             float Ns,
             string map_Kd);
};
