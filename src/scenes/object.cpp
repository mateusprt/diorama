#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "object.h"
#include "material.h"

//STB_IMAGE
#include <stb_image/stb_image.h>

using namespace std;

Object::Object(const string& objPath,
               const string& texPath,
               const string& mtlPath)
{
    VAO = loadSimpleOBJ(objPath, nVertices);
    mtl = loadMTL(mtlPath);
    int w, h;
    texID = loadTexture(texPath, w, h);
}

GLuint Object::loadTexture(const string& filePath, int &width, int &height) {
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
		cout << "Failed to load texture " << filePath << endl;
		cerr << "stb_image load error: " << stbi_failure_reason() << endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

int Object::loadSimpleOBJ(string filePath, int &nVertices) {
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

Material Object::loadMTL(const string& filename) {
    Material mat;
    ifstream file(filename);
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