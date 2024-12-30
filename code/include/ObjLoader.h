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

#include "utils.h"
#include "Light.h"
#include "Camera.h"

struct ObjectRenderMetaData {
    GLuint diffuse;
    GLuint specular;
    GLuint normalMap;
    GLuint emission;
    GLuint alpha;
    GLsizei size;
    size_t start_idx;
};

class ObjLoader
{
public:
    ObjLoader() : VAO(0), VBO(0), EBO(0), shaderProgram(0) {}
    ~ObjLoader();

    bool load(const std::string &objPath, const std::string &materialRootPath, const char *vertPath, const char *fragPath);
    void renderWithTexture(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model, std::vector<Light> lights, Light env_light);
    void renderWithColor(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model, const glm::vec3 &color, const float transparency);
    

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    std::vector<float> vertices;                                    // 每个顶点包含位置、法线和纹理坐标
    std::vector<unsigned int> indices;                              // 顶点索引数组
    std::vector<int> materialIndices;                               // 顶点材质的索引
    std::unordered_map<int, GLuint> materialToDiffuseMap;           // 材质索引到漫反射纹理ID的映射（同高光）
    std::unordered_map<int, GLuint> materialToEmissionMap;          // 材质索引到发射贴图ID的映射
    std::unordered_map<int, GLuint> materialToAlphaMap;             // 材质索引到透明度贴图ID的映射
    std::unordered_map<std::string, int> DiffuseNameToID;           // 漫反射纹理文件名到材质索引的映射
    std::unordered_map<std::string, int> EmissionNameToID;          // 发射纹理文件名到材质索引的映射
    std::unordered_map<std::string, int> AlphaNameToID;             // 透明度纹理文件名到材质索引的映射
    std::vector<ObjectRenderMetaData> textureRenderList;            // 每批次纹理渲染元数据列表

    GLuint loadTexture(const std::string &texturePath);
    GLuint loadShader(const char *vertexPath, const char *fragmentPath);
};

#endif
