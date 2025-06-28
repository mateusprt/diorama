#pragma once

#include <vector>

#include "object.h"
#include "shader.h"
#include "camera.h"
#include "mouse.h"

// GLFW
#include <GLFW/glfw3.h>

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
     * @brief Prepara todos os recursos
     * Deve ser chamado uma vez na inicialização - antes do "game loop".
     */
    void prepare();

    /**
     * @brief Desenha a cena usando o shader e a câmera configurada
     * Deve ser chamado no loop principal de render.
     */
    void draw(GLFWwindow *window);

    glm::vec3 circularPath(float t, float radius, float height);

    void destroy();

    GLuint generateFloor();

    bool loadConfig(const std::string& configPath);

    std::vector<Object> mObjects;
    Camera              mCamera;
    Shader              mShader;
    float               deltaTime;
    float               lastFrame;
    bool                rotateX;
    bool                rotateY;
    bool                rotateZ;
    GLint               viewLoc;
    GLint               projLoc;
    GLint               modelLoc;
    GLint               normalMatLoc;
    int                 selectedObject;
    Mouse               mouse;
    GLuint              floorTexID;
    GLuint              floorVAO;
    float               lastX;
    float               lastY;
};
