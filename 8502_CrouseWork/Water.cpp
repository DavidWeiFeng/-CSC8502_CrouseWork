#include "Water.h"
#include <iostream>

/**
 * 构造函数 - 创建水面
 *
 * @param position 水面中心位置（世界坐标）
 * @param size 水面大小（宽度和深度，单位：世界坐标）
 */
Water::Water(const glm::vec3& position, float size)
    : position(position), size(size), VAO(0), VBO(0), EBO(0)
{
    setupWater();
    std::cout << "水面创建成功：" << std::endl;
    std::cout << "  位置：(" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    std::cout << "  大小：" << size << "x" << size << std::endl;
}

/**
 * 析构函数 - 释放OpenGL资源
 */
Water::~Water() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

/**
 * 初始化水面平面
 *
 * 水面是一个简单的水平矩形，由2个三角形组成
 * 法向量指向上方 (0, 1, 0)
 */
void Water::setupWater() {
    // ========================================
    // 计算水面的四个顶点位置
    // ========================================
    // 水面以 position 为中心，大小为 size x size
    float halfSize = size / 2.0f;

    // 四个顶点的位置（相对于中心）
    float minX = position.x - halfSize;
    float maxX = position.x + halfSize;
    float y = position.y;  // 水面高度
    float minZ = position.z - halfSize;
    float maxZ = position.z + halfSize;

    // ========================================
    // 顶点数据：位置、法向量、纹理坐标
    // ========================================
    // 格式：x, y, z, nx, ny, nz, u, v
    // 法向量全部指向上方 (0, 1, 0)
    float vertices[] = {
        // 位置 (x, y, z)           法向量 (nx, ny, nz)    纹理坐标 (u, v)
        // ====================================================================
        // 顶点0：左下角
        minX, y, minZ,              0.0f, 1.0f, 0.0f,       0.0f, 0.0f,
        // 顶点1：右下角
        maxX, y, minZ,              0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        // 顶点2：右上角
        maxX, y, maxZ,              0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        // 顶点3：左上角
        minX, y, maxZ,              0.0f, 1.0f, 0.0f,       0.0f, 1.0f
    };

    // ========================================
    // 索引数据：定义两个三角形
    // ========================================
    // 三角形1：0-1-2（左下 -> 右下 -> 右上）
    // 三角形2：0-2-3（左下 -> 右上 -> 左上）
    unsigned int indices[] = {
        0, 1, 2,  // 第一个三角形
        0, 2, 3   // 第二个三角形
    };

    // ========================================
    // 创建并配置VAO、VBO、EBO
    // ========================================
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 绑定VAO
    glBindVertexArray(VAO);

    // 配置VBO（顶点数据）
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 配置EBO（索引数据）
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ========================================
    // 配置顶点属性指针
    // ========================================
    // 每个顶点：3个float(位置) + 3个float(法向量) + 2个float(纹理坐标) = 8个float
    int stride = 8 * sizeof(float);

    // 属性0：顶点位置 (x, y, z)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // 属性1：法向量 (nx, ny, nz)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    // 属性2：纹理坐标 (u, v)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    // 解绑VAO
    glBindVertexArray(0);
}

/**
 * 渲染水面
 *
 * 注意：
 * - 着色器和uniform设置应该在调用此函数之前完成
 * - 如果需要半透明效果，应该启用混合并最后渲染
 */
void Water::Render() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // 6个索引（2个三角形）
    glBindVertexArray(0);
}
