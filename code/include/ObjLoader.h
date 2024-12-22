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



struct ObjectRenderMetaData {
    GLuint texture;
    GLsizei size;
    size_t start_idx;
};

class ObjLoader {
public:
    ObjLoader() : VAO(0), VBO(0), EBO(0), shaderProgram(0) {}
    ~ObjLoader();

    bool load(const std::string& objPath, const std::string& mtlBasePath, const char* vertPath, const char* fragPath);
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model);
    void renderWithColor(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color, const float transparency);
    GLuint loadTexture(const std::string& texturePath);

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<int> materialIndices; // 每个面对应的材质索引
    std::unordered_map<int, GLuint> materialTextures; // 材质索引 -> 纹理ID映射
    std::vector<ObjectRenderMetaData> textureRenderList;

    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};


#endif
