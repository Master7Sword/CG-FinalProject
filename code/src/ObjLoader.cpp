#include <thread>
#include <mutex>

#include "ObjLoader.h"
#include "utils.h"

ObjLoader::~ObjLoader()
{
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (EBO)
        glDeleteBuffers(1, &EBO);

    for (const auto &[materialID, textureID] : materialToTextureMap)
    {
        glDeleteTextures(1, &textureID);
    }
}

bool ObjLoader::load(const std::string &objPath, const std::string &materialRootPath, const char *vertPath, const char *fragPath)
{
    tinyobj::attrib_t vertexAttr;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // 加载OBJ文件
    if (!tinyobj::LoadObj(&vertexAttr, &shapes, &materials, &warn, &err, objPath.c_str(), materialRootPath.c_str()))
    {
        std::cerr << "Failed to load OBJ file!" << std::endl;
        std::cerr << "[Warning] " << warn << std::endl;
        std::cerr << "[Error] " << err << std::endl;
        return false;
    }
    if (!warn.empty())
    {
        std::cout << "Warning: " << warn << std::endl;
    }

    // 加载材质的漫反射纹理
    // #pragma omp parallel for
    for (size_t i = 0; i < materials.size(); ++i)
    {
        if (!materials[i].diffuse_texname.empty())
        {
            std::string texturePath = materialRootPath + "/" + materials[i].diffuse_texname;
            if(textureNameToID[texturePath] != 0)
            {
                materialToTextureMap[i] = textureNameToID[texturePath];
            }
            else
            {
                GLuint textureID = loadTexture(texturePath);
                if (textureID)
                {
                    materialToTextureMap[i] = textureID; // 保存材质索引和纹理ID的映射
                    textureNameToID[texturePath] = textureID;
                    std::cout << "Loaded texture for material " << i << " at " << texturePath << std::endl;
                }
                else
                {
                    std::cerr << "Failed to load texture for material " << i << " at " << texturePath << std::endl;
                }
            }
        }
    }

    // 加载顶点数据
    // #pragma omp parallel for
    for (const auto &shape : shapes)
    {
        size_t indexOffset = 0; // vertices 数组中的索引偏移
        for (size_t i = 0; i < shape.mesh.num_face_vertices.size(); ++i)
        {
            int materialID = shape.mesh.material_ids[i];
            materialIndices.push_back(materialID);

            int vertexNum = shape.mesh.num_face_vertices[i];
            for (size_t j = 0; j < vertexNum; ++j)
            {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + j];
                // 读取顶点坐标
                glm::vec3 vertexCoord(
                    vertexAttr.vertices[3 * idx.vertex_index + 0],
                    vertexAttr.vertices[3 * idx.vertex_index + 1],
                    vertexAttr.vertices[3 * idx.vertex_index + 2]);
                glm::vec2 texCoord(0.0f, 0.0f);
                // 读取纹理坐标
                if (idx.texcoord_index >= 0)
                {
                    texCoord = glm::vec2(
                        vertexAttr.texcoords[2 * idx.texcoord_index + 0],
                        vertexAttr.texcoords[2 * idx.texcoord_index + 1]);
                }

                vertices.insert(vertices.end(), {vertexCoord.x, vertexCoord.y, vertexCoord.z, texCoord.x, texCoord.y});
                indices.push_back(indices.size());
            }
            indexOffset += vertexNum;
        }
    }

    // 创建纹理渲染列表，连续的相同纹理顶点放入同一批次
    GLuint lastTextureID = -1;
    size_t batchStartIdx = 0;
    size_t vertexNum = materialIndices.size();
    for (size_t idx = 0; idx < vertexNum; idx++)
    {
        GLuint currentTextureID;

        int materialID = materialIndices[idx];
        auto it = materialToTextureMap.find(materialID);
        if (it != materialToTextureMap.end())
            currentTextureID = it->second;
        else
            currentTextureID = 0;

        if (currentTextureID != lastTextureID)
        {
            if (idx - batchStartIdx > 0)
            {
                // 将上一批次的顶点数据加入渲染列表，每个顶点3个元素
                textureRenderList.push_back(ObjectRenderMetaData{lastTextureID, GLsizei(idx - batchStartIdx) * 3, batchStartIdx * 3}); // 每个顶点3个元素
            }
            batchStartIdx = idx;
            lastTextureID = currentTextureID;
        }
    }
    // 处理最后一批次
    if (vertexNum - 1 - batchStartIdx > 0)
    {
        textureRenderList.push_back(ObjectRenderMetaData{lastTextureID, GLsizei(vertexNum - 1 - batchStartIdx) * 3, batchStartIdx * 3});
    }

    // 创建VAO、VBO、EBO对象
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // 绑定VAO
    glBindVertexArray(VAO);
    // 绑定并填充VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    // 绑定并填充EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    // 配置顶点空间坐标属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 配置顶点纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 解绑VAO
    glBindVertexArray(0);

    // 加载着色器
    shaderProgram = loadShader(vertPath, fragPath);

    return true;
}

GLuint ObjLoader::loadTexture(const std::string &texturePath)
{
    // 创建纹理
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载图像
    int width, height, channelNum;
    unsigned char *imgData = stbi_load(texturePath.c_str(), &width, &height, &channelNum, 0); // 保持原始通道数
    if (imgData)
    {
        // 生成纹理
        GLenum format = (channelNum == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        std::cerr << "Failed to load texture at " << texturePath << std::endl;

    // 释放内存
    stbi_image_free(imgData);

    return textureID;
}

GLuint ObjLoader::loadShader(const char *vertexPath, const char *fragmentPath)
{
    // 读取着色器代码（必须设置中间变量，否则会被立即释放）
    std::string vertexShaderSource = readFile(vertexPath);
    std::string fragmentShaderSource = readFile(fragmentPath);
    const char *vertexShaderCode = vertexShaderSource.c_str();
    const char *fragmentShaderCode = fragmentShaderSource.c_str();

    // 创建并编译顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);

    // 创建并编译片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);

    // 创建着色器程序
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // 释放着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void ObjLoader::renderWithTexture(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model)
{
    // 加载着色器程序
    glUseProgram(shaderProgram);

    // 设置矩阵
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // 渲染每个纹理批次
    glBindVertexArray(VAO);
    for (ObjectRenderMetaData &meta_data : textureRenderList)
    {
        GLuint texture = meta_data.texture;
        GLsizei size = meta_data.size;
        size_t start_idx = meta_data.start_idx;
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void *)(start_idx * sizeof(unsigned int)));
    }
    glBindVertexArray(0);
}

void ObjLoader::renderWithColor(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model, const glm::vec3 &color, const float transparency)
{
    glUseProgram(shaderProgram);

    // 设置矩阵
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // 设置颜色
    glUniform3fv(glGetUniformLocation(shaderProgram, "overrideColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocationARB(shaderProgram, "transparency"), transparency);

    // 给每个顶点设置颜色
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
