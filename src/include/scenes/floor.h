// src/shared/floor.h
#pragma once

#include <string>
#include "object.h"

using namespace std;

/**
 * @brief Classe Floor: quad no plano Y=0 herdando de Object
 */
class Floor : public Object {
public:
    /**
     * @brief Construtor que carrega a textura
     * @param texturePath Caminho da textura do piso
     */
    Floor(const string& texturePath);

private:
    /**
     * @brief Gera o VAO do piso
     */
    GLuint generateFloor();
};