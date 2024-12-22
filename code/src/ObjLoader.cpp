#include "ObjLoader.h"
#include "utils.h"

ObjLoader::~ObjLoader() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);

    for (const auto& [materialID, textureID] : materialTextures) {
        glDeleteTextures(1, &textureID);
    }
}

bool ObjLoader::load(const std::string& objPath, const std::string& mtlBasePath, const char* vertPath, const char* fragPath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), mtlBasePath.c_str())) {
        std::cerr << "Failed to load OBJ file: " << warn << err << std::endl;
        return false;
    }
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    // 加载材质的纹理
    for (size_t i = 0; i < materials.size(); ++i) {
        if (!materials[i].diffuse_texname.empty()) {
            std::string texturePath = mtlBasePath + "/" + materials[i].diffuse_texname;
            GLuint textureID = loadTexture(texturePath);
            if (textureID) {
                materialTextures[i] = textureID; // 保存材质索引和纹理ID的映射
                std::cout << "Loaded texture for material " << i << ": " << texturePath << std::endl;
            } else {
                std::cerr << "Failed to load texture for material " << i << ": " << texturePath << std::endl;
            }
        }
    }

    // 加载顶点数据
    for (const auto& shape : shapes) {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            int materialID = shape.mesh.material_ids[f];
            materialIndices.push_back(materialID);

            int fv = shape.mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; ++v) {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];

                glm::vec3 vertex(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
                glm::vec2 texCoord(0.0f, 0.0f);
                if (idx.texcoord_index >= 0) {
                    texCoord = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                }

                vertices.insert(vertices.end(), { vertex.x, vertex.y, vertex.z, texCoord.x, texCoord.y });
                indices.push_back(indices.size());
            }
            indexOffset += fv;
        }
    }

    // 创建 VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    shaderProgram = loadShader(vertPath, fragPath);

    return true;
}

GLuint ObjLoader::loadTexture(const std::string& texturePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载图像
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

GLuint ObjLoader::loadShader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void ObjLoader::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model) {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);

    size_t indexOffset = 0;
    for (size_t i = 0; i < materialIndices.size(); ++i) {
        int materialID = materialIndices[i];
        auto it = materialTextures.find(materialID);
        if (it != materialTextures.end()) {
            glBindTexture(GL_TEXTURE_2D, it->second);
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(indexOffset * sizeof(unsigned int)));
        indexOffset += 3;
    }

    glBindVertexArray(0);
}

void ObjLoader::renderWithColor(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color, const float transparency) {
    glUseProgram(shaderProgram);

    // 设置矩阵
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // 设置颜色
    glUniform3fv(glGetUniformLocation(shaderProgram, "overrideColor"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocationARB(shaderProgram, "transparency"), transparency);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
