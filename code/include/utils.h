#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <SFML/Audio.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "stb_image.h"


// 发射音效
extern sf::SoundBuffer launchBuffer;
extern sf::SoundBuffer explosionBuffer;
extern sf::Sound launchSound;
extern sf::Sound explosionSound;

GLuint loadShader(const char* vertexPath, const char* fragmentPath);
GLuint loadTexture(const std::string& path);
std::string readFile(const std::string& filePath);
glm::vec3 getRandomColor();

#endif
