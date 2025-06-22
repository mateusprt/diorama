#pragma once
#include <string>
#include <glad/glad.h>

class Texture {
public:
    GLuint texID;
    GLuint load(const std::string& filePath, int &width, int &height);
};
