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

// Protótipos das funções
void initializeGLFW();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int setupGeometry();
int loadSimpleOBJ(string filePATH, int &nVertices);
GLuint loadTexture(string filePath, int &width, int &height);
Material loadMTL(const std::string& filename);
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
const string MTL_PATH = "../assets/models/rat/model_1.mtl";

GLFWwindow* window;

Object objects[NUM_OBJECTS];

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

	GLint viewLoc = glGetUniformLocation(shader.ID, "view");	
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");

	for (size_t i = 0; i < NUM_OBJECTS; i++) {
		objects[i].VAO = loadSimpleOBJ(OBJ_PATH, objects[i].nVertices);
		int texWidth,texHeight;
		objects[i].texID = loadTexture(TEXTURE_PATH, texWidth, texHeight);
	}

	// espalha os três modelos na cena
	objects[0].offsetX = -1.0f;
	objects[1].offsetX =  0.0f;
	objects[2].offsetX =  1.0f;

	glUseProgram(shader.ID); 

	// Matriz identidade; //posição inicial do obj no mundo
	glm::mat4 model = glm::mat4(1);
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Buffer de textura no shader
	glUniform1i(glGetUniformLocation(shader.ID, "texBuffer"), 0);
	GLint normalMatLoc = glGetUniformLocation(shader.ID, "normalMatrix");

	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	shader.setFloat("ka",0.2);
	shader.setFloat("ks", 0.5);
	shader.setFloat("kd", 0.5);
	shader.setFloat("q", 10.0);

	//Propriedades da fonte de luz
	shader.setVec3("lightPos",-2.0, 10.0, 3.0);
	shader.setVec3("lightColor",1.0, 1.0, 1.0);

	// chão
	int floorW, floorH;
  GLuint floorTexID = loadTexture("../assets/models/floor/concrete.jpg", floorW, floorH);
	GLuint floorVAO = generateFloor();

	//  queijo
	Object cheese;
	cheese.VAO = loadSimpleOBJ("../assets/models/cheese/cheese.obj", cheese.nVertices);
	cheese.mtl = loadMTL("../assets/models/cheese/cheese.mtl");
	int texWidth,texHeight;
	cheese.texID = loadTexture("../assets/models/cheese/" + cheese.mtl.map_Kd, texWidth, texHeight);
	cheese.scale = 0.5f;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor

		// Matriz de view
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		 //Matriz de projeção
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glLineWidth(10);
		glPointSize(20);

		// desenhando o chão
		// Modelo do chão: sem translação ou escala extra (já está em –5…+5)
		glm::mat4 floorModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(floorModel));
		// normalMatrix para piso (na inclinação identidade, fica mat3(1))
		glm::mat3 nmFloor = glm::mat3(1.0f);
		glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(nmFloor));

		// bind do VAO do chão
		glBindVertexArray(floorVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexID);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// desenhando os ratos
		float angle = (GLfloat)glfwGetTime();
		for (size_t i = 0; i < NUM_OBJECTS; i++) {
			objects[i].offsetY = 0.45f; // ajuste para o objeto "pisar" no chão
			objects[i].model = glm::mat4(1);
			objects[i].model = glm::scale(objects[i].model, glm::vec3(objects[i].scale, objects[i].scale, objects[i].scale));
			objects[i].model = glm::translate(objects[i].model, glm::vec3(objects[i].offsetX, objects[i].offsetY, objects[i].offsetZ));

			if (rotateX) {
			objects[selectedObject].model = glm::rotate(objects[selectedObject].model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
			} else if (rotateY) {
				objects[selectedObject].model = glm::rotate(objects[selectedObject].model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
			} else if (rotateZ) {
				objects[selectedObject].model = glm::rotate(objects[selectedObject].model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objects[i].model));

			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES
			glBindVertexArray(objects[i].VAO);
			glBindTexture(GL_TEXTURE_2D, objects[i].texID);
			glDrawArrays(GL_TRIANGLES, 0, objects[i].nVertices);
		}

		// desenhando o queijo
    float radius = 2.0f;                        // raio do círculo
    float height = 0.45f;                       // altura fixa sobre o chão
    glm::vec3 pos = circularPath(currentFrame, radius, height);
    float speed = glm::radians(45.0f); 
    float angleCheese = speed * currentFrame; // ângulo em radianos
    glm::mat4 cheeseModel = glm::mat4(1.0f);
    cheeseModel = glm::translate(cheeseModel, pos); // curva
    cheeseModel = glm::rotate   (cheeseModel, angleCheese, glm::vec3(0,1,0));
    cheeseModel = glm::scale    (cheeseModel, glm::vec3(cheese.scale));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cheeseModel));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cheese.texID);
    glBindVertexArray(cheese.VAO);

    glDrawArrays(GL_TRIANGLES, 0, cheese.nVertices);

    glBindVertexArray(0);

		//Atualizar a matriz de view
		//Matriz de view
		view = glm::lookAt(cameraPos, cameraPos + cameraFront,cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		
		//Propriedades da câmera
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	for (size_t i = 0; i < NUM_OBJECTS; i++) {
		// Desalocando os buffers
		glDeleteVertexArrays(1, &objects[i].VAO);
	}

	glDeleteVertexArrays(1, &floorVAO);
	glDeleteVertexArrays(1, &cheese.VAO);
	
	glfwTerminate();
	return 0;
}

glm::vec3 circularPath(float t, float radius, float height) {
	return glm::vec3(
			radius * std::cos(t),
			height,
			radius * std::sin(t)
	);
}

GLuint generateFloor() {
	
	// 4 vértices de um quad no plano Y=0
	float floorVerts[] = {
		//  pos.x, pos.y, pos.z,   norm.x, norm.y, norm.z,   tex.s, tex.t
		-5.0f, 0.0f, -5.0f,      0.0f, 1.0f, 0.0f,         0.0f, 0.0f,
			5.0f, 0.0f, -5.0f,      0.0f, 1.0f, 0.0f,         5.0f, 0.0f,
			5.0f, 0.0f,  5.0f,      0.0f, 1.0f, 0.0f,         5.0f, 5.0f,
		-5.0f, 0.0f,  5.0f,      0.0f, 1.0f, 0.0f,         0.0f, 5.0f
	};
	unsigned int floorIdx[] = {
		0,1,2,
		0,2,3
	};

	GLuint floorVAO, floorVBO, floorEBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glGenBuffers(1, &floorEBO);

	glBindVertexArray(floorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerts), floorVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIdx), floorIdx, GL_STATIC_DRAW);

	// pos (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// cor (location = 1) — você pode passar uma cor fixa no shader ou usar finalColor
	// textura (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// normal (location = 3)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	
	return floorVAO;
}

void initializeGLFW() {
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	window = glfwCreateWindow(WIDTH, HEIGHT, "Diorama *__*", nullptr, nullptr);
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

	/* if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS) {
		objects[selectedObject].offsetY += 0.5f;
	}
	if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS) {
		objects[selectedObject].offsetY -= 0.5f;
	}
	if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
		objects[selectedObject].offsetX -= 0.5f;
	}
	if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
		objects[selectedObject].offsetX += 0.5f;
	}
	if ((key == GLFW_KEY_Q || key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
		objects[selectedObject].offsetZ -= 0.5f;
	}
	if ((key == GLFW_KEY_E || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
		objects[selectedObject].offsetZ += 0.5f;
	}

	if ((key == GLFW_KEY_F || key == GLFW_KEY_LEFT) && action == GLFW_PRESS) {
		objects[selectedObject].scale -= 0.25f;
	}
	if ((key == GLFW_KEY_G || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) {
		objects[selectedObject].scale += 0.25f;
	} */

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
  cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  fov -= (float)yoffset;
  fov = glm::clamp(fov,  1.0f, 45.0f);
}

void processInput(GLFWwindow *window) {
	float speed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += speed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= speed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			cameraPos -= cameraUp * speed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			cameraPos += cameraUp * speed;
}

int loadSimpleOBJ(string filePath, int &nVertices)
{
	vector <glm::vec3> vertices;
	vector <glm::vec2> texCoords;
	vector <glm::vec3> normals;
	vector <GLfloat> vBuffer;

	glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);

	ifstream arqEntrada;

	arqEntrada.open(filePath.c_str());
	if (arqEntrada.is_open())
	{
		//Fazer o parsing
		string line;
		while (!arqEntrada.eof())
		{
			getline(arqEntrada,line);
			istringstream ssline(line);
			string word;
			ssline >> word;
			if (word == "v")
			{
				glm::vec3 vertice;
				ssline >> vertice.x >> vertice.y >> vertice.z;
				//cout << vertice.x << " " << vertice.y << " " << vertice.z << endl;
				vertices.push_back(vertice);

			}
			if (word == "vt")
			{
				glm::vec2 vt;
				ssline >> vt.s >> vt.t;
				//cout << vertice.x << " " << vertice.y << " " << vertice.z << endl;
				texCoords.push_back(vt);

			}
			if (word == "vn")
			{
				glm::vec3 normal;
				ssline >> normal.x >> normal.y >> normal.z;
				//cout << vertice.x << " " << vertice.y << " " << vertice.z << endl;
				normals.push_back(normal);

			}
			else if (word == "f") {
				// 1) junte todos os índices da face
				vector<string> refs;
				string ref;
				while (ssline >> ref)
						refs.push_back(ref);

				// 2) para cada triângulo (fan): [0,i,i+1]
				for (size_t i = 1; i + 1 < refs.size(); ++i) {
					string tri[3] = { refs[0], refs[i], refs[i+1] };
					for (int k = 0; k < 3; ++k) {
						// parse tri[k] no formato vi/ti/ni
						istringstream ss(tri[k]);
						int vi, ti, ni;
						string idx;
						getline(ss, idx, '/');  vi = stoi(idx) - 1;
						getline(ss, idx, '/');  ti = stoi(idx) - 1;
						getline(ss, idx);       ni = stoi(idx) - 1;

						// empurra pro vBuffer: posição, cor, UV, normal
						vBuffer.push_back(vertices[vi].x);
						vBuffer.push_back(vertices[vi].y);
						vBuffer.push_back(vertices[vi].z);

						// cor fixa (ou sua cor do material)
						vBuffer.push_back(color.r);
						vBuffer.push_back(color.g);
						vBuffer.push_back(color.b);

						vBuffer.push_back(texCoords[ti].s);
						vBuffer.push_back(texCoords[ti].t);

						vBuffer.push_back(normals[ni].x);
						vBuffer.push_back(normals[ni].y);
						vBuffer.push_back(normals[ni].z);
					}
				}
			}
		}

		arqEntrada.close();

		cout << "Gerando o buffer de geometria..." << endl;
		GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura - s, t
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//Atributo vetor normal - x, y, z
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8*sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	nVertices = vBuffer.size() / 11;
	return VAO;

	}
	else
	{
		cout << "Erro ao tentar ler o arquivo " << filePath << endl;
		return -1;
	}
}

GLuint loadTexture(string filePath, int &width, int &height)
{
	GLuint texID; // id da textura a ser carregada

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Ajuste dos parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carregamento da imagem usando a função stbi_load da biblioteca stb_image
	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) // jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else // assume que é 4 canais png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << filePath << std::endl;
		std::cerr << "stb_image load error: " << stbi_failure_reason() << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

Material loadMTL(const std::string& filename) {
    Material mat;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        
      if (key == "Ka") iss >> mat.Ka.r >> mat.Ka.g >> mat.Ka.b;
			else if (key == "Kd") iss >> mat.Kd.r >> mat.Kd.g >> mat.Kd.b;
			else if (key == "Ks") iss >> mat.Ks.r >> mat.Ks.g >> mat.Ks.b;
			else if (key == "Ns") iss >> mat.Ns;
			else if (key == "map_Kd") iss >> mat.map_Kd;
    }
    
    return mat;
}