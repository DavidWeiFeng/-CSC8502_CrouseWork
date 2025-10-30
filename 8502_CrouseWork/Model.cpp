#include "Model.h"
#include "Texture.h"
#include <iostream>

// ========================================
// 构造函数
// ========================================
Model::Model(const std::string& filePath)
    : m_Loaded(false)
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "开始加载模型: " << filePath << std::endl;
    std::cout << "========================================" << std::endl;

    // 提取模型文件所在目录（用于加载纹理）
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos)
    {
        m_Directory = filePath.substr(0, lastSlash + 1);
    }

    // 加载模型文件
    m_Loaded = LoadModel(filePath);

    if (m_Loaded)
    {
        std::cout << "✓ 模型加载成功！" << std::endl;
        std::cout << "  - 网格数量: " << m_Meshes.size() << std::endl;
    }
    else
    {
        std::cerr << "✗ 模型加载失败！" << std::endl;
    }

    std::cout << "========================================\n" << std::endl;
}

// ========================================
// 析构函数
// ========================================
Model::~Model()
{
    std::cout << "[Model] 模型已销毁" << std::endl;
}

// ========================================
// 加载模型（支持多种格式）
// ========================================
// 工作原理：
// 1. 使用 Assimp 读取模型文件
// 2. 递归遍历场景图的所有节点
// 3. 提取顶点、法向量、纹理坐标
// 4. 创建Mesh对象
// ========================================
bool Model::LoadModel(const std::string& filePath)
{
    // ========================================
    // 步骤1：创建 Assimp Importer
    // ========================================
    Assimp::Importer importer;

    // ========================================
    // 步骤2：读取文件并应用后处理
    // ========================================
    // aiProcess_Triangulate: 将所有多边形转换为三角形
    // aiProcess_FlipUVs: 翻转 Y 轴的纹理坐标（OpenGL 约定）
    // aiProcess_GenSmoothNormals: 生成平滑法向量
    // aiProcess_CalcTangentSpace: 计算切线和副切线（用于法线贴图）
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace
    );

    // ========================================
    // 步骤3：检查错误
    // ========================================
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp 错误: " << importer.GetErrorString() << std::endl;
        return false;
    }

    std::cout << "[LoadModel] 文件解析成功" << std::endl;
    std::cout << "  - 网格数: " << scene->mNumMeshes << std::endl;
    std::cout << "  - 材质数: " << scene->mNumMaterials << std::endl;
    std::cout << "  - 纹理数: " << scene->mNumTextures << std::endl;

    // ========================================
    // 步骤4：递归处理场景图的所有节点
    // ========================================
    ProcessNode(scene->mRootNode, scene);

    return true;
}

// ========================================
// 递归处理节点（场景图遍历）
// ========================================
void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    std::cout << "[ProcessNode] 处理节点: " << node->mName.C_Str() << std::endl;

    // ========================================
    // 处理当前节点的所有网格
    // ========================================
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        // node->mMeshes[i] 是场景网格数组的索引
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* processedMesh = ProcessMesh(mesh, scene);
        if (processedMesh)
        {
            m_Meshes.push_back(std::unique_ptr<Mesh>(processedMesh));
        }
    }

    // ========================================
    // 递归处理所有子节点
    // ========================================
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

// ========================================
// 处理单个网格
// ========================================
Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::cout << "  [ProcessMesh] 网格名称: " << mesh->mName.C_Str() << std::endl;

    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;

    // ========================================
    // 步骤1：处理顶点数据
    // ========================================
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Mesh::Vertex vertex;

        // 位置
        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // 法向量
        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        // 纹理坐标（只取第一套 UV）
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoord = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            vertex.TexCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    std::cout << "    - 顶点数: " << vertices.size() << std::endl;

    // ========================================
    // 步骤2：处理索引数据
    // ========================================
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    std::cout << "    - 三角形数: " << indices.size() / 3 << std::endl;

    // ========================================
    // 步骤3：处理材质和纹理
    // ========================================
    std::shared_ptr<Texture> texture = nullptr;

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 获取材质名称
        aiString matName;
        material->Get(AI_MATKEY_NAME, matName);
        std::cout << "    - 材质: " << matName.C_Str() << std::endl;

        // 加载漫反射纹理
        texture = LoadMaterialTexture(material, aiTextureType_DIFFUSE);
    }

    // ========================================
    // 步骤4：创建 Mesh 对象
    // ========================================
    return new Mesh(vertices, indices, texture);
}

// ========================================
// 加载材质纹理
// ========================================
std::shared_ptr<Texture> Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type)
{
    // 检查是否有该类型的纹理
    if (mat->GetTextureCount(type) == 0)
    {
        std::cout << "      - 无纹理" << std::endl;
        return nullptr;
    }

    // 获取第一个纹理的路径
    aiString path;
    mat->GetTexture(type, 0, &path);

    std::string texturePath = m_Directory + std::string(path.C_Str());
    std::cout << "      - 加载纹理: " << path.C_Str() << std::endl;

    try
    {
        auto texture = std::make_shared<Texture>(texturePath);
        if (texture->IsLoaded())
        {
            std::cout << "      ✓ 纹理加载成功" << std::endl;
            return texture;
        }
        else
        {
            std::cerr << "      ✗ 纹理加载失败" << std::endl;
            return nullptr;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "      ✗ 纹理加载异常: " << e.what() << std::endl;
        return nullptr;
    }
}

// ========================================
// 渲染模型
// ========================================
void Model::Render()
{
    // 渲染所有网格
    for (auto& mesh : m_Meshes)
    {
        mesh->Render();
    }
}
