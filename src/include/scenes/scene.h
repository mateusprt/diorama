#pragma once

#include <vector>
#include "object.h"
#include "shader.h"
#include "camera.h"

/**
 * @brief Gerencia a cena 3D: prepara recursos e desenha todos os objetos
 */
class Scene {
public:
    /**
     * @brief Construtor que inicializa a cena com o shader
     * @param shader  Shader program para renderização
     */
    Scene(const Shader& shader);

    ~Scene();

    /**
     * @brief Prepara todos os recursos (shaders, texturas, meshes)
     * Deve ser chamado uma vez na inicialização.
     */
    void prepare();

    /**
     * @brief Desenha a cena usando o shader e a câmera configurada
     * Deve ser chamado no loop principal de render.
     */
    void draw();

private:
    std::vector<Object> mObjects;
    Camera              mCamera;
    Shader              mShader;
};
