#include "Sky.h"

Sky::~Sky()
{
    if (VAO)  glDeleteVertexArrays(1, &VAO);
    if (VBO)  glDeleteBuffers(1, &VBO);
    if (EBO)  glDeleteBuffers(1, &EBO);

    for (const auto &[materialID, textureID] : materialToDiffuseMap)
        glDeleteTextures(1, &textureID);
}

bool Sky::load(const std::string &objPath, const std::string &materialRootPath, const char *vertPath, const char *fragPath)
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

    for (size_t i = 0; i < materials.size(); ++i) {
        const auto &material = materials[i];

        // 漫反射贴图
        if (!material.diffuse_texname.empty()) {
            std::string texturePath = materialRootPath + "/" + material.diffuse_texname;
            if (DiffuseNameToID[texturePath] != 0) {
                materialToDiffuseMap[i] = DiffuseNameToID[texturePath];
            } else {
                GLuint textureID = loadTexture(texturePath);
                if (textureID) {
                    materialToDiffuseMap[i] = textureID; 
                    DiffuseNameToID[texturePath] = textureID;
                    std::cout << "Loaded diffuse texture for material " << i << " at " << texturePath << std::endl;
                } else {
                    std::cerr << "Failed to load diffuse texture for material " << i << " at " << texturePath << std::endl;
                }
            }
        }
    }

    // 加载顶点数据
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
                // aPos
                glm::vec3 vertexCoord(
                    vertexAttr.vertices[3 * idx.vertex_index + 0],
                    vertexAttr.vertices[3 * idx.vertex_index + 1],
                    vertexAttr.vertices[3 * idx.vertex_index + 2]);
                glm::vec2 texCoord(0.0f, 0.0f);
                // aTexCoord
                if (idx.texcoord_index >= 0)
                {
                    texCoord = glm::vec2(
                        vertexAttr.texcoords[2 * idx.texcoord_index + 0],
                        1.0f - vertexAttr.texcoords[2 * idx.texcoord_index + 1]);   // OpenGL原点在左下角，blender在左上角，需要翻转
                }

                vertices.insert(vertices.end(), {vertexCoord.x, vertexCoord.y, vertexCoord.z, texCoord.x, texCoord.y});
                indices.push_back(indices.size());
            }
            indexOffset += vertexNum;
        }
    }

    // 创建纹理渲染列表，连续的相同纹理顶点放入同一批次
    GLuint lastDiffuseID = -1;
    size_t batchStartIdx = 0;
    size_t vertexNum = materialIndices.size();

    for (size_t idx = 0; idx < vertexNum; idx++)
    {
        // 当前材质对应的各类纹理ID
        GLuint currentDiffuseID = 0;

        int materialID = materialIndices[idx];

        // 查询漫反射贴图ID
        auto itDiffuse = materialToDiffuseMap.find(materialID);
        if (itDiffuse != materialToDiffuseMap.end())
            currentDiffuseID = itDiffuse->second;

        // 如果任何一种材质发生切换，则保存当前批次信息
        if (currentDiffuseID != lastDiffuseID)
        {
            if (idx - batchStartIdx > 0)
            {
                // 保存上一批次
                textureRenderList.push_back(SkyRenderMetaData{
                    lastDiffuseID,
                    GLsizei(idx - batchStartIdx) * 3, // 顶点数量 * 每顶点属性数
                    batchStartIdx * 3                // 起始顶点位置
                });
            }
            // 更新批次起始点
            batchStartIdx = idx;
            lastDiffuseID = currentDiffuseID;
        }
    }

    // 处理最后一批次
    if (vertexNum - batchStartIdx > 0)
    {
        textureRenderList.push_back(SkyRenderMetaData{
            lastDiffuseID,
            GLsizei(vertexNum - batchStartIdx) * 3,
            batchStartIdx * 3
        });
    }

    // 创建VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 加载着色器
    shaderProgram = loadShader(vertPath, fragPath);
    if (!shaderProgram) {
        std::cerr << "Failed to load shader program!" << std::endl;
        return false;
    }

    return true;
}


void Sky::render(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model)
{
    glUseProgram(shaderProgram);

    // 设置矩阵
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    // 绑定VAO
    glBindVertexArray(VAO);

    // 渲染每个纹理批次
    for (const auto &renderMeta : textureRenderList)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderMeta.diffuse);
        glUniform1i(glGetUniformLocation(shaderProgram, "skyTexture"), 0);

        glDrawElements(GL_TRIANGLES, renderMeta.size, GL_UNSIGNED_INT, (void*)(renderMeta.start_idx * sizeof(unsigned int)));
    }

    glBindVertexArray(0);
}

GLuint Sky::loadTexture(const std::string &texturePath)
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
    if (imgData) {
        GLenum format;
        if (channelNum == 1) {
            format = GL_RED; // 灰度图
        } else if (channelNum == 3) {
            format = GL_RGB; // RGB 图像
        } else if (channelNum == 4) {
            format = GL_RGBA; // RGBA 图像
        } else {
            std::cerr << "Unsupported channel count: " << channelNum << " in texture " << texturePath << std::endl;
            stbi_image_free(imgData);
            return 0;
        }

        // 上传纹理数据到 GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 如果是灰度图，设置纹理采样行为
        if (channelNum == 1) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
        }
    }
    else
        std::cerr << "Failed to load texture at " << texturePath << std::endl;

    // 释放内存
    stbi_image_free(imgData);

    return textureID;
}

GLuint Sky::loadShader(const char *vertexPath, const char *fragmentPath)
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
