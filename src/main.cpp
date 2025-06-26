#include <iostream>
#include <string>
#include <assert.h>

#include <vector>
#include <fstream>
#include <sstream>

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
int setupGeometry();
int loadSimpleOBJ(string filePATH, int &nVertices);
GLuint loadTexture(string filePath, int &width, int &height);
//Material loadMTL(const std::string& filename);
glm::vec3 circularPath(float t, float radius, float height);
GLuint generateFloor();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;
bool rotateX=false, rotateY=false, rotateZ=false;
const int NUM_OBJECTS = 3;

//Variáveis globais da câmera
glm::vec3 cameraPos = glm::vec3(0.0f, 1.1f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0,-1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
Camera camera = Camera(cameraPos, cameraFront, cameraUp);

//–– tempo para velocidade constante ––
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//–– mouse ––
float yaw   = -90.0f; // inicial apontando no -Z
float pitch =   0.0f;
float lastX =  WIDTH  / 2.0f;
float lastY =  HEIGHT / 2.0f;
bool  firstMouse = true;
float fov =  45.0f;


const GLchar* VERTEX_SHADER_PATH = "../shaders/phong.vs";
const GLchar* FRAGMENT_SHADER_PATH = "../shaders/phong.fs";

const string OBJ_PATH = "../assets/models/rat/model.obj";
const string TEXTURE_PATH = "../assets/models/rat/texture.jpeg";
const string MTL_PATH = "../assets/models/rat/model.mtl";

#include <iostream>
#include <unistd.h>

GLFWwindow* window;

int selectedObject = 0;

int main()
{

	initializeGLFW();

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader(VERTEX_SHADER_PATH,FRAGMENT_SHADER_PATH);

	Scene mySecene = Scene(shader);
	mySecene.prepare();

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window)) {
		mySecene.draw(window);
	}

	mySecene.destroy();
	
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
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float speed = 5.0f * deltaTime;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		selectedObject = 0;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		selectedObject = 1;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos; lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // invertido
  lastX = xpos; lastY = ypos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw   += xoffset;
  pitch += yoffset;
  pitch = glm::clamp(pitch, -89.0f, 89.0f);

  glm::vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera.front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  fov = glm::clamp(fov,  1.0f, 45.0f);
}