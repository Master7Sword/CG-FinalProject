#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <unordered_map>
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ObjectRenderMetaData
{
    GLuint texture;
    GLsizei size;
    size_t start_idx;
};

class ObjLoader
{
public:
    ObjLoader() : VAO(0), VBO(0), EBO(0), shaderProgram(0) {}
    ~ObjLoader();

    bool load(const std::string &objPath, const std::string &materialRootPath, const char *vertPath, const char *fragPath);
    void renderWithTexture(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model);
    void renderWithColor(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model, const glm::vec3 &color, const float transparency);
    GLuint loadTexture(const std::string &texturePath);

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    std::vector<float> vertices;                          // 顶点坐标数组，每个顶点占用5个元素
    std::vector<unsigned int> indices;                    // 顶点索引数组，用于创建EBO
    std::vector<int> materialIndices;                     // 顶点材质的索引
    std::unordered_map<int, GLuint> materialToTextureMap; // 材质索引到纹理ID的映射
    std::vector<ObjectRenderMetaData> textureRenderList;  // 纹理渲染列表
    std::unordered_map<std::string, int> textureNameToID; // 纹理文件名到材质索引的映射

    GLuint loadShader(const char *vertexPath, const char *fragmentPath);
};

#endif
