#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "material.h"

using namespace std;


class Object {
public:
    GLuint VAO;        // Índice do buffer de geometria
    GLuint texID;      // Identificador da textura carregada
    int nVertices;     // Número de vértices

    // paths
    string path;
    string mtlPath;
    string texturePath;

    string type;

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

    /**
     * @brief Construtor que carrega modelo OBJ, textura e material
     * @param objPath Caminho para arquivo .obj
     * @param texPath Caminho para arquivo de textura
     * @param mtlPath Caminho para arquivo .mtl
     */
    Object(const string& objPath,
           const string& texPath,
           const string& mtlPath);
    
    /**
     * @brief Carrega um modelo OBJ simples para um VAO.
     * @param filePath   Caminho para o arquivo .obj
     * @param nVertices  Retorna o número de vértices carregados
     * @return GLuint    ID do VAO gerado
     */
    int loadSimpleOBJ(string filePATH, int &nVertices);

    /**
     * @brief Carrega uma textura de arquivo usando stb_image.
     * @param filePath Caminho para o arquivo de imagem.
     * @param width    Retorna a largura da imagem.
     * @param height   Retorna a altura da imagem.
     * @return GLuint  ID da textura OpenGL.
     */
    GLuint loadTexture(const std::string& filePath, int &width, int &height);

     /**
     * @brief Carrega material de um arquivo .mtl
     * @param filename Caminho para o arquivo .mtl
     * @return Material carregado
     */
    Material loadMTL(const std::string& filename);
};