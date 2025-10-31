#pragma once
#include <glad/glad.h>
#include "nclgl/Matrix4.h"
#include <string>
#include <vector>

// 使用 nclgl 框架的 Shader
#include "nclgl/Shader.h"

/**
 * @class Skybox
 * @brief 天空盒类 - 使用立方体贴图渲染无限远的天空背景
 *
 * 功能：
 * - 加载6张纹理组成立方体贴图（上、下、左、右、前、后）
 * - 渲染时去除相机平移，只保留旋转（保持无限远感觉）
 * - 始终渲染在场景最远处（深度值 = 1.0）
 * - 提供立方体贴图ID用于环境映射
 */
class Skybox {
public:
    /**
     * @brief 构造函数 - 加载立方体贴图纹理
     * @param faces 6张纹理的路径，顺序必须是：
     *              [0] +X (右), [1] -X (左)
     *              [2] +Y (上), [3] -Y (下)
     *              [4] +Z (前), [5] -Z (后)
     */
    Skybox(const std::vector<std::string>& faces);

    /**
     * @brief 析构函数 - 释放OpenGL资源
     */
    ~Skybox();

    /**
     * @brief 渲染天空盒
     * @param shader 天空盒专用着色器
     * @param view 相机视图矩阵
     * @param projection 投影矩阵
     */
    void Draw(Shader& shader, const Matrix4& view, const Matrix4& projection);

    /**
     * @brief 获取立方体贴图纹理ID
     * @return OpenGL纹理ID，可用于环境映射
     */
    GLuint GetCubemapID() const { return cubemapTexture; }

private:
    GLuint VAO;              // 顶点数组对象
    GLuint VBO;              // 顶点缓冲对象
    GLuint cubemapTexture;   // 立方体贴图纹理ID

    /**
     * @brief 初始化天空盒立方体的顶点数据和VAO/VBO
     */
    void setupSkybox();

    /**
     * @brief 加载立方体贴图
     * @param faces 6张纹理的路径
     * @return 立方体贴图的纹理ID
     */
    GLuint loadCubemap(const std::vector<std::string>& faces);
};
