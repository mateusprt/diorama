#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "material.h"


class Object {
public:
    GLuint VAO;        // Índice do buffer de geometria
    GLuint texID;      // Identificador da textura carregada
    int nVertices;     // Número de vértices

    // paths
    string path;
    string mtlPath;
    string texturePath;

    // Transformações e material
    glm::mat4 model;   // Matriz de transformações do objeto
    float ka;          // coeficiente ambiente
    float kd;          // coeficiente difuso
    float ks;          // coeficiente especular
    Material mtl;      // propriedades do material

    // Parâmetros de posição e escala do modelo
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float offsetZ = 0.0f;
    float scale   = 1.0f;

    // Construtor padrão (sem inicializar explicitamente todos os membros)
    Object();
};