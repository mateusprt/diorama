#include "floor.h"
#include "object.h"
#include <string>
// GLAD
#include <glad/glad.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

Floor::Floor(const string& texturePath): Object("", texturePath, "") {
    // Carrega textura
    int width, height;
    texID = loadTexture(texturePath, width, height);

    VAO = generateFloor();
}

GLuint Floor::generateFloor() {
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
