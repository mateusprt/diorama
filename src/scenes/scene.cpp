#include "scene.h"
#include "camera.h"
#include "mouse.h"
#include "functions.h"

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Classe gerenciadora dos shaders
#include "Shader.h"

#include "constants.h"

Scene::Scene(const Shader& shader)
    : mCamera(
        glm::vec3(0.0f, 1.1f, 8.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
      ),
      mShader(shader),
			mouse(
				/* yaw   */ -90.0f,
        /* pitch */   0.0f,
        /* lastX */ Constants::WINDOW_WIDTH  / 2.0f,
        /* lastY */ Constants::WINDOW_HEIGHT / 2.0f,
        /* firstMouse */ true,
        /* fov   */  45.0f
			)
{
  selectedObject = 0;
	rotateX = false;
	rotateY = false;
	rotateZ = false;
	lastX =  Constants::WINDOW_WIDTH  / 2.0f;
	lastY =  Constants::WINDOW_HEIGHT  / 2.0f;
}

Scene::~Scene() = default;

void Scene::prepare() {
	glUseProgram(mShader.ID);

	viewLoc = glGetUniformLocation(mShader.ID, "view");	
	projLoc = glGetUniformLocation(mShader.ID, "projection");

	// chão

	int floorW, floorH;
  floorTexID = loadTexture("../assets/models/floor/concrete.jpg", floorW, floorH);
	floorVAO = generateFloor();

	// ratos
	const string OBJ_PATH = "../assets/models/rat/model.obj";
	const string TEXTURE_PATH = "../assets/models/rat/texture.jpeg";
	const string MTL_PATH = "../assets/models/rat/model.mtl";
  for (size_t i = 0; i < 3; i++) {
		Object obj = Object(OBJ_PATH, TEXTURE_PATH, MTL_PATH);
		mObjects.push_back(obj);
	}

	//  queijo
	Object cheese = Object("../assets/models/cheese/cheese.obj", "../assets/models/cheese/texture.png", "../assets/models/cheese/cheese.mtl");
	cheese.scale = 0.5f;
	mObjects.push_back(cheese);

	// espalha os três modelos de ratos na cena
	mObjects[0].offsetX = -1.0f;
	mObjects[1].offsetX =  0.0f;
	mObjects[2].offsetX =  1.0f;

	// Matriz identidade; //posição inicial do obj no mundo
	glm::mat4 model = glm::mat4(1);
	modelLoc = glGetUniformLocation(mShader.ID, "model");
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Buffer de textura no shader
	glUniform1i(glGetUniformLocation(mShader.ID, "texBuffer"), 0);
	normalMatLoc = glGetUniformLocation(mShader.ID, "normalMatrix");

	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	mShader.setFloat("ka",0.2);
	mShader.setFloat("ks", 0.5);
	mShader.setFloat("kd", 0.5);
	mShader.setFloat("q", 10.0);

	//Propriedades da fonte de luz
	mShader.setVec3("lightPos",-2.0, 10.0, 3.0);
	mShader.setVec3("lightColor",1.0, 1.0, 1.0);
}

void Scene::draw(GLFWwindow *window) {
	glUseProgram(mShader.ID);
  float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
	glfwPollEvents();

	processInput(window);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor

	// Configurando visão da cena/câmera
	// Matriz de view
	glm::mat4 view = glm::lookAt(mCamera.pos, mCamera.pos + mCamera.front, mCamera.up);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//Matriz de projeção
	glm::mat4 projection = glm::perspective(glm::radians(mouse.fov), (float)Constants::WINDOW_WIDTH / Constants::WINDOW_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glLineWidth(10);
	glPointSize(20);

	// Desenhando
	
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
	for (size_t i = 0 ; i < mObjects.size()-1; i++) {
		mObjects[i].offsetY = 0.45f; // ajuste para o objeto "pisar" no chão
		mObjects[i].model = glm::mat4(1);
		mObjects[i].model = glm::scale(mObjects[i].model, glm::vec3(mObjects[i].scale, mObjects[i].scale, mObjects[i].scale));
		mObjects[i].model = glm::translate(mObjects[i].model, glm::vec3(mObjects[i].offsetX, mObjects[i].offsetY, mObjects[i].offsetZ));

		if (rotateX) {
		mObjects[selectedObject].model = glm::rotate(mObjects[selectedObject].model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		} else if (rotateY) {
			mObjects[selectedObject].model = glm::rotate(mObjects[selectedObject].model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		} else if (rotateZ) {
			mObjects[selectedObject].model = glm::rotate(mObjects[selectedObject].model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mObjects[i].model));

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		glBindVertexArray(mObjects[i].VAO);
		glBindTexture(GL_TEXTURE_2D, mObjects[i].texID);
		glDrawArrays(GL_TRIANGLES, 0, mObjects[i].nVertices);
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
	cheeseModel = glm::scale    (cheeseModel, glm::vec3(mObjects[mObjects.size()-1].scale));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cheeseModel));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mObjects[mObjects.size()-1].texID);
	glBindVertexArray(mObjects[mObjects.size()-1].VAO);
	glDrawArrays(GL_TRIANGLES, 0, mObjects[mObjects.size()-1].nVertices);

	glBindVertexArray(0);

	//Atualizar a matriz de view
	//Matriz de view
	view = mCamera.getViewMatrix();//::lookAt(cameraPos, cameraPos + cameraFront,cameraUp);
	glUniformMatrix4fv(glGetUniformLocation(mShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	
	//Propriedades da câmera
	mShader.setVec3("cameraPos", mCamera.pos.x, mCamera.pos.y, mCamera.pos.z);

	// Troca os buffers da tela
	glfwSwapBuffers(window);
}

void Scene::processInput(GLFWwindow *window) {
	float speed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			mCamera.pos += speed * mCamera.front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			mCamera.pos -= speed * mCamera.front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			mCamera.pos -= glm::normalize(glm::cross(mCamera.front, mCamera.up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			mCamera.pos += glm::normalize(glm::cross(mCamera.front, mCamera.up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			mCamera.pos -= mCamera.up * speed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			mCamera.pos += mCamera.up * speed;
}

glm::vec3 Scene::circularPath(float t, float radius, float height) {
	return glm::vec3(
			radius * std::cos(t),
			height,
			radius * std::sin(t)
	);
}

void Scene::destroy() {
	for(int x = 0; x < mObjects.size(); x++) {
		glDeleteVertexArrays(1, &mObjects[x].VAO);
	}
}

GLuint Scene::generateFloor() {
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