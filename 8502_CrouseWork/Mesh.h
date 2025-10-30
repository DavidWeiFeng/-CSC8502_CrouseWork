#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

// 前向声明
class Texture;

// ========================================
// Mesh类 - 表示一个可渲染的3D网格
// ========================================
// 功能：
// 1. 存储顶点数据（位置、法向量、纹理坐标）
// 2. 存储索引数据（三角形）
// 3. 管理OpenGL缓冲对象（VAO/VBO/EBO）
// 4. 渲染网格
// ========================================

class Mesh
{
public:
    // ========================================
    // 顶点结构体
    // ========================================
    // 注意：必须与Terrain的Vertex结构体保持一致
    // 这样可以使用相同的着色器
    // ========================================
    struct Vertex
    {
        glm::vec3 Position;   // 顶点位置 (x, y, z)
        glm::vec3 Normal;     // 法向量（用于光照）
        glm::vec2 TexCoord;   // 纹理坐标 (u, v)
    };

    // ========================================
    // 构造函数
    // ========================================
    // 参数：
    //   vertices - 顶点数组
    //   indices  - 索引数组（三角形）
    //   texture  - 纹理指针（可选，默认nullptr）
    // ========================================
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         std::shared_ptr<Texture> texture = nullptr);

    // ========================================
    // 析构函数 - 清理OpenGL资源
    // ========================================
    ~Mesh();

    // ========================================
    // 渲染网格
    // ========================================
    // 注意：调用前必须激活着色器并设置uniform
    // ========================================
    void Render();

    // ========================================
    // 获取顶点和索引数量（用于调试）
    // ========================================
    size_t GetVertexCount() const { return m_Vertices.size(); }
    size_t GetIndexCount() const { return m_Indices.size(); }

    // ========================================
    // 纹理管理
    // ========================================
    void SetTexture(std::shared_ptr<Texture> texture) { m_Texture = texture; }
    std::shared_ptr<Texture> GetTexture() const { return m_Texture; }
    bool HasTexture() const { return m_Texture != nullptr; }

private:
    // ========================================
    // 网格数据
    // ========================================
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    // ========================================
    // OpenGL对象
    // ========================================
    GLuint m_VAO;  // 顶点数组对象
    GLuint m_VBO;  // 顶点缓冲对象
    GLuint m_EBO;  // 索引缓冲对象

    // ========================================
    // 纹理
    // ========================================
    std::shared_ptr<Texture> m_Texture;  // 漫反射纹理（从材质加载）

    // ========================================
    // 私有函数：设置OpenGL缓冲
    // ========================================
    void SetupMesh();
};

#endif // MESH_H
