#include "Skybox.h"
#include <iostream>

// STB 库（用于加载纹理）
#include "stb_image.h"

/**
 * 构造函数 - 加载立方体贴图并初始化顶点数据
 */
Skybox::Skybox(const std::vector<std::string>& faces) {
    // 加载立方体贴图纹理
    cubemapTexture = loadCubemap(faces);

    // 初始化天空盒立方体的顶点数据
    setupSkybox();
}

/**
 * 析构函数 - 释放OpenGL资源
 */
Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &cubemapTexture);
}

/**
 * 初始化天空盒立方体
 *
 * 天空盒是一个以原点为中心的单位立方体（边长2，从-1到+1）
 * 只需要顶点位置，不需要纹理坐标和法向量
 * 顶点位置本身就作为采样立方体贴图的方向向量
 */
void Skybox::setupSkybox() {
    // 天空盒立方体的36个顶点（6个面 × 2个三角形 × 3个顶点）
    // 注意：顶点顺序很重要，确保正面朝内（因为相机在立方体内部）
    float skyboxVertices[] = {
        // 位置坐标 (x, y, z)
        // ============================

        // 后面 (Back face, -Z方向)
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // 左面 (Left face, -X方向)
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // 右面 (Right face, +X方向)
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        // 前面 (Front face, +Z方向)
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // 上面 (Top face, +Y方向)
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        // 下面 (Bottom face, -Y方向)
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // 创建并配置VAO和VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // 顶点位置属性 (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 解绑
    glBindVertexArray(0);
}

/**
 * 加载立方体贴图
 *
 * @param faces 6张纹理的路径，顺序必须是：
 *              [0] +X (右), [1] -X (左)
 *              [2] +Y (上), [3] -Y (下)
 *              [4] +Z (前), [5] -Z (后)
 * @return 立方体贴图的纹理ID
 */
GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    std::cout << "开始加载天空盒立方体贴图..." << std::endl;

    // 加载6张纹理到立方体贴图的6个面
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        // 使用 STB 加载图像（立方体贴图不需要翻转Y轴）
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 3);  // 强制RGB

        if (data) {
            // 根据通道数确定格式
            GLenum format = GL_RGB;  // 默认使用RGB
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            // 加载纹理数据到对应的立方体贴图面
            // GL_TEXTURE_CUBE_MAP_POSITIVE_X + i 枚举值是连续的
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,  // 目标面
                0,                                     // Mipmap级别
                format,                                // 内部格式
                width, height,                         // 宽度和高度
                0,                                     // 边框（必须为0）
                format,                                // 数据格式
                GL_UNSIGNED_BYTE,                      // 数据类型
                data                                   // 像素数据
            );

            stbi_image_free(data);

            std::cout << "天空盒纹理加载成功: " << faces[i] << " (" << width << "x" << height << ")" << std::endl;
        }
        else {
            std::cerr << "错误：立方体贴图纹理加载失败: " << faces[i] << std::endl;
            std::cerr << "STB错误信息: " << stbi_failure_reason() << std::endl;
        }
    }

    // 设置纹理参数
    // 使用线性过滤，让天空盒更平滑
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 使用CLAMP_TO_EDGE防止立方体边缘出现接缝
    // 这确保纹理坐标超出范围时不会采样到对面的纹理
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

/**
 * 渲染天空盒
 *
 * 渲染技巧：
 * 1. 去除view矩阵的平移分量（在着色器中实现）
 * 2. 设置深度值为1.0（最远）（在着色器中实现）
 * 3. 在渲染循环中最后渲染天空盒
 * 4. 使用glDepthFunc(GL_LEQUAL)允许深度值为1.0通过
 *
 * @param shader 天空盒专用着色器
 * @param view 相机视图矩阵
 * @param projection 投影矩阵
 */
void Skybox::Draw(Shader& shader, const Matrix4& view, const Matrix4& projection) {
    // 安全检查：确保VAO已经创建
    if (VAO == 0) {
        std::cerr << "错误：天空盒VAO未初始化，无法渲染！" << std::endl;
        return;
    }

    // 激活着色器
    glUseProgram(shader.GetProgram());

    // 设置uniform变量（nclgl Shader 使用 glUniform 直接设置）
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "view"), 1, false, (float*)&view);
    glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "projection"), 1, false, (float*)&projection);

    // 绑定天空盒立方体贴图到纹理单元0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(shader.GetProgram(), "skybox"), 0);

    // 渲染立方体
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);  // 36个顶点（6面 × 2三角形 × 3顶点）
    glBindVertexArray(0);
}
