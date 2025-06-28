#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

#include "Dependencies.h"

// Classes
#include "object.h"
#include "material.h"
#include "camera.h"
#include "scene.h"

// Protótipos das funções
void initializeGLFW();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

GLFWwindow* window;
Scene* myScene = nullptr;

int main() {

	initializeGLFW();

	// Compilando e buildando o programa de shader
	Shader shader("../shaders/phong.vs","../shaders/phong.fs");
	myScene = new Scene(shader);
	myScene->prepare();

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window)) {
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();
		processInput(window);
		myScene->draw(window);
	}

	myScene->destroy();
	delete myScene;
	glfwTerminate();
	return 0;
}


void initializeGLFW() {
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	window = glfwCreateWindow(WIDTH, HEIGHT, "Banquete giratório", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// Desabilita cursor e captura movimento
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Callback de mouse
	glfwSetCursorPosCallback(window, mouse_callback);
	// Callback de scroll (para zoom)
	glfwSetScrollCallback(window, scroll_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	
	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		myScene->rotateX = true;
		myScene->rotateY = false;
		myScene->rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		myScene->rotateX = false;
		myScene->rotateY = true;
		myScene->rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		myScene->rotateX = false;
		myScene->rotateY = false;
		myScene->rotateZ = true;
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		myScene->selectedObject = 0;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		myScene->selectedObject = 1;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		myScene->selectedObject = 2;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	/**
		a posição atual (xpos,ypos) - anterior (lastX,lastY) = o quanto o mouse se moveu em cada eixo. 
	*/
  if (myScene->mouse.firstMouse) {
    myScene->lastX = xpos; myScene->lastY = ypos;
    myScene->mouse.firstMouse = false;
  }
  float xoffset = xpos - myScene->lastX;
  float yoffset = myScene->lastY - ypos; // invertido
  myScene->lastX = xpos; myScene->lastY = ypos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  // Soma os offsets aos ângulos de orientação (yaw e pitch) da câmera, e limita o pitch para não virar de cabeça para baixo
  myScene->mouse.yaw += xoffset;
  myScene->mouse.pitch += yoffset;
  myScene->mouse.pitch = glm::clamp(myScene->mouse.pitch, -89.0f, 89.0f);

  // front diz para onde a câmera está olhando, e é usado em glm::lookAt para montar a matriz de visão
  glm::vec3 front;
  front.x = cos(glm::radians(myScene->mouse.yaw)) * cos(glm::radians(myScene->mouse.pitch));
  front.y = sin(glm::radians(myScene->mouse.pitch));
  front.z = sin(glm::radians(myScene->mouse.yaw)) * cos(glm::radians(myScene->mouse.pitch));
  myScene->mCamera.front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  myScene->mouse.fov -= (float)yoffset;
  myScene->mouse.fov = glm::clamp(myScene->mouse.fov,  1.0f, 45.0f);
}

void processInput(GLFWwindow *window) {
	float speed = 5.0f * myScene->deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			myScene->mCamera.pos += speed * myScene->mCamera.front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			myScene->mCamera.pos -= speed * myScene->mCamera.front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			myScene->mCamera.pos -= glm::normalize(glm::cross(myScene->mCamera.front, myScene->mCamera.up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			myScene->mCamera.pos += glm::normalize(glm::cross(myScene->mCamera.front, myScene->mCamera.up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			myScene->mCamera.pos -= myScene->mCamera.up * speed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			myScene->mCamera.pos += myScene->mCamera.up * speed;
}