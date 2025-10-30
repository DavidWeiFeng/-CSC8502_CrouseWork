#include "Mesh.h"
#include "Texture.h"
#include <iostream>

// ========================================
// 构造函数
// ========================================
Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           std::shared_ptr<Texture> texture)
    : m_Vertices(vertices)
    , m_Indices(indices)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_Texture(texture)
{
    std::cout << "[Mesh] 创建网格："
              << vertices.size() << " 顶点, "
              << indices.size() / 3 << " 三角形";

    if (m_Texture)
    {
        std::cout << " (带纹理)";
    }
    std::cout << std::endl;

    // 设置OpenGL缓冲对象
    SetupMesh();
}

// ========================================
// 析构函数
// ========================================
Mesh::~Mesh()
{
    // 删除OpenGL对象
    if (m_VAO != 0)
        glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0)
        glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0)
        glDeleteBuffers(1, &m_EBO);

    std::cout << "[Mesh] 网格已销毁" << std::endl;
}

// ========================================
// 设置OpenGL缓冲
// ========================================
// 工作原理：
// 1. 创建VAO（记录顶点属性配置）
// 2. 创建VBO（存储顶点数据）
// 3. 创建EBO（存储索引数据）
// 4. 配置顶点属性指针
// ========================================
void Mesh::SetupMesh()
{
    // ========================================
    // 步骤1：生成并绑定VAO
    // ========================================
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // ========================================
    // 步骤2：创建VBO，上传顶点数据
    // ========================================
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_Vertices.size() * sizeof(Vertex),
                 m_Vertices.data(),
                 GL_STATIC_DRAW);

    // ========================================
    // 步骤3：创建EBO，上传索引数据
    // ========================================
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_Indices.size() * sizeof(unsigned int),
                 m_Indices.data(),
                 GL_STATIC_DRAW);

    // ========================================
    // 步骤4：配置顶点属性指针
    // ========================================
    // Vertex结构体布局：
    // [Position(12字节), Normal(12字节), TexCoord(8字节)] = 32字节

    // 属性0：位置 (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)0);
    glEnableVertexAttribArray(0);

    // 属性1：法向量 (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // 属性2：纹理坐标 (vec2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, TexCoord));
    glEnableVertexAttribArray(2);

    // ========================================
    // 步骤5：解绑VAO
    // ========================================
    glBindVertexArray(0);

    std::cout << "  - VAO: " << m_VAO
              << ", VBO: " << m_VBO
              << ", EBO: " << m_EBO << std::endl;
}

// ========================================
// 渲染网格
// ========================================
void Mesh::Render()
{
    // ========================================
    // 如果有纹理，绑定到纹理单元0
    // ========================================
    if (m_Texture)
    {
        m_Texture->Bind(0);
    }

    // 绑定VAO
    glBindVertexArray(m_VAO);

    // 使用索引绘制三角形
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(m_Indices.size()),
                   GL_UNSIGNED_INT,
                   0);

    // 解绑VAO
    glBindVertexArray(0);
}
