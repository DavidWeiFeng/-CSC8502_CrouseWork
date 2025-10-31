#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include <string>
#include <vector>
#include <memory>

// Assimp 头文件
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ========================================
// Model类 - 从多种格式加载3D模型
// ========================================
// 功能：
// 1. 使用 Assimp 加载多种模型格式（OBJ, FBX, GLTF, 等）
// 2. 解析顶点、法向量、纹理坐标
// 3. 创建Mesh对象
// 4. 渲染模型
// ========================================

class Model
{
public:
    // ========================================
    // 构造函数
    // ========================================
    // 参数：
    //   filePath - 模型文件路径（支持 OBJ, FBX, GLTF, 等多种格式）
    // ========================================
    Model(const std::string& filePath);

    // ========================================
    // 析构函数
    // ========================================
    ~Model();

    // ========================================
    // 渲染模型
    // ========================================
    // 注意：调用前必须激活着色器并设置uniform
    // ========================================
    void Render();

    // ========================================
    // 获取模型信息（用于调试）
    // ========================================
    bool IsLoaded() const { return m_Loaded; }
    size_t GetMeshCount() const { return m_Meshes.size(); }

private:
    // ========================================
    // 模型数据
    // ========================================
    std::vector<std::unique_ptr<Mesh>> m_Meshes;  // 所有网格
    std::vector<std::shared_ptr<Texture>> m_Textures;  // 所有纹理（从材质加载）
    bool m_Loaded;                                 // 是否成功加载
    std::string m_Directory;                       // 模型文件所在目录（用于加载纹理）

    // ========================================
    // 私有函数：Assimp 加载流程
    // ========================================
    bool LoadModel(const std::string& filePath);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::shared_ptr<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, const aiScene* scene);
};

#endif // MODEL_H
