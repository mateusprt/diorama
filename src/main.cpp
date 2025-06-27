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

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

//Variáveis globais da câmera
glm::vec3 cameraPos = glm::vec3(0.0f, 1.1f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0,-1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
Camera camera = Camera(cameraPos, cameraFront, cameraUp);

const GLchar* VERTEX_SHADER_PATH = "../shaders/phong.vs";
const GLchar* FRAGMENT_SHADER_PATH = "../shaders/phong.fs";

GLFWwindow* window;
Scene* myScene = nullptr;

int main()
{

	initializeGLFW();

	// Compilando e buildando o programa de shader
	Shader shader(VERTEX_SHADER_PATH,FRAGMENT_SHADER_PATH);
	myScene = new Scene(shader);
	myScene->prepare();

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window)) {
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
	window = glfwCreateWindow(WIDTH, HEIGHT, "Diorama", nullptr, nullptr);
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
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

  myScene->mouse.yaw += xoffset;
  myScene->mouse.pitch += yoffset;
  myScene->mouse.pitch = glm::clamp(myScene->mouse.pitch, -89.0f, 89.0f);

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