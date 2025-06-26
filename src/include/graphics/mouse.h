#pragma once

/**
 * @brief Gerencia o estado do mouse para controle de câmera
 */
class Mouse {
public:
    float yaw;         ///< ângulo yaw (em graus)
    float pitch;       ///< ângulo pitch (em graus)
    float lastX;       ///< última posição X do cursor
    float lastY;       ///< última posição Y do cursor
    bool  firstMouse;  ///< indica se é o primeiro evento de mouse
    float fov;         ///< campo de visão

    /**
     * @brief Construtor que define valores iniciais ao estado do mouse
     * @param initYaw      Valor inicial de yaw
     * @param initPitch    Valor inicial de pitch
     * @param initLastX    Valor inicial de lastX
     * @param initLastY    Valor inicial de lastY
     * @param initFirst    Valor inicial de firstMouse
     * @param initFov      Valor inicial de fov
     */
    Mouse(float yaw,
          float pitch,
          float lastX,
          float lastY,
          bool  firstMouse,
          float fov);
        
    ~Mouse();
};