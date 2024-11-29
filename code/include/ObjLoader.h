#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ObjLoader {
public:
    ObjLoader() : VAO(0), VBO(0), EBO(0), texture(0) {}
    ~ObjLoader();

    bool load(const std::string& objPath, const std::string& mtlBasePath);
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model);
    GLuint loadTexture(const std::string& texturePath);

private:
    GLuint VAO, VBO, EBO, texture;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint shaderProgram;

    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};

#endif
