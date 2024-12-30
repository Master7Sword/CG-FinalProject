#ifndef SKY_H
#define SKY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include "stb_image.h"
#include "utils.h"

struct SkyRenderMetaData {
    GLuint diffuse;
    GLsizei size;
    size_t start_idx;
};

class Sky{
public:
    Sky(): VAO(0), VBO(0), EBO(0), shaderProgram(0) {}
    ~Sky();

    bool load(const std::string &objPath, const std::string &materialRootPath, const char *vertPath, const char *fragPath);
    void render(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model);

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;

    std::vector<float> vertices;                      
    std::vector<unsigned int> indices;                            
    std::vector<int> materialIndices;
    std::unordered_map<int, GLuint> materialToDiffuseMap;
    std::unordered_map<std::string, int> DiffuseNameToID;
    std::vector<SkyRenderMetaData> textureRenderList;

    GLuint loadTexture(const std::string& path);
    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};

#endif