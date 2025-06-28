#version 430
layout (location = 0) in vec3 position; // posição no espaço do objeto
layout (location = 1) in vec3 color; // posição trimestral (x,y,z)
layout (location = 2) in vec2 texc; // coordenada de textura (u,v)
layout (location = 3) in vec3 normal; // vetor normal por vértice

uniform mat4 model; // posiciona
uniform mat4 projection; // posicionamento da câmera
uniform mat4 view; // projeta 3d p/ 2d

//Variáveis que irão para o fragment shader
out vec3 finalColor; // cor interpolada
out vec2 texCoord;
out vec3 scaledNormal; // normal transformada ao espaço mundo
out vec3 fragPos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0); // Cálculo da posição final na tela
	finalColor = color;
    texCoord = vec2(texc.s, 1 - texc.t);
    fragPos = vec3(model * vec4(position, 1.0)); // posição do fragmento em espaço mundo
    scaledNormal = vec3(model * vec4(normal, 1.0));
}