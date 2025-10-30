# Assimp 迁移指南

## 概述

本文档说明如何将项目中的模型加载库从 **tiny_obj_loader** 迁移到 **Assimp (Open Asset Import Library)**。

### 为什么使用 Assimp？

| 特性 | tiny_obj_loader | Assimp |
|------|----------------|--------|
| **支持格式** | 仅 OBJ | 40+ 格式（FBX, GLTF, Collada, 3DS, Blend 等） |
| **材质支持** | 基础 MTL 支持 | 完整的 PBR 材质支持 |
| **动画支持** | ❌ 不支持 | ✅ 支持骨骼动画、关键帧动画 |
| **场景图** | ❌ 无 | ✅ 内置层级结构 |
| **法向量/切线** | 需手动计算 | ✅ 自动生成 |
| **库大小** | 极小（单头文件） | 较大（需要链接库） |

**结论**：Assimp 功能更强大，适合复杂项目和多格式支持。

---

## 第一步：下载和配置 Assimp

### 1.1 下载预编译库

**推荐方式**：使用预编译版本

1. 访问 [Assimp Releases](https://github.com/assimp/assimp/releases)
2. 下载最新版本的 Windows SDK（例如 `assimp-5.3.1-windows-sdk.zip`）
3. 解压到 `8502_CrouseWork/Dependencies/Assimp/` 目录

**目录结构**：
```
8502_CrouseWork/
└── Dependencies/
    └── Assimp/
        ├── include/          # 头文件
        │   └── assimp/
        │       ├── Importer.hpp
        │       ├── scene.h
        │       ├── postprocess.h
        │       └── ...
        ├── lib/              # 库文件
        │   ├── x64/
        │   │   ├── Debug/
        │   │   │   └── assimp-vc143-mtd.lib
        │   │   └── Release/
        │   │       └── assimp-vc143-mt.lib
        │   └── x86/ (可选)
        └── bin/              # DLL 文件
            ├── x64/
            │   ├── Debug/
            │   │   └── assimp-vc143-mtd.dll
            │   └── Release/
            │       └── assimp-vc143-mt.dll
            └── x86/ (可选)
```

**注意**：
- `vc143` 代表 Visual Studio 2022（VS2019 使用 `vc142`）
- `mt` = Multi-threaded, `mtd` = Multi-threaded Debug
- **必须将对应的 DLL 文件复制到可执行文件目录**

---

### 1.2 配置 Visual Studio 项目

#### 方法 1：手动编辑项目文件（推荐）

编辑 `8502_CrouseWork.vcxproj`，在 `<PropertyGroup>` 之后添加：

```xml
<!-- Assimp 配置 -->
<ItemDefinitionGroup>
  <ClCompile>
    <AdditionalIncludeDirectories>$(ProjectDir)Dependencies\Assimp\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
  </ClCompile>
</ItemDefinitionGroup>

<ItemDefinitionGroup Condition="'$(Configuration)'=='Debug'">
  <Link>
    <AdditionalLibraryDirectories>$(ProjectDir)Dependencies\Assimp\lib\x64\Debug;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    <AdditionalDependencies>assimp-vc143-mtd.lib;%(AdditionalDependencies)</AdditionalDependencies>
  </Link>
</ItemDefinitionGroup>

<ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
  <Link>
    <AdditionalLibraryDirectories>$(ProjectDir)Dependencies\Assimp\lib\x64\Release;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    <AdditionalDependencies>assimp-vc143-mt.lib;%(AdditionalDependencies)</AdditionalDependencies>
  </Link>
</ItemDefinitionGroup>
```

#### 方法 2：使用 Visual Studio GUI

1. 右键项目 → 属性
2. **C/C++** → **常规** → **附加包含目录**：
   ```
   $(ProjectDir)Dependencies\Assimp\include
   ```
3. **链接器** → **常规** → **附加库目录**（Debug）：
   ```
   $(ProjectDir)Dependencies\Assimp\lib\x64\Debug
   ```
4. **链接器** → **常规** → **附加库目录**（Release）：
   ```
   $(ProjectDir)Dependencies\Assimp\lib\x64\Release
   ```
5. **链接器** → **输入** → **附加依赖项**：
   - Debug: `assimp-vc143-mtd.lib`
   - Release: `assimp-vc143-mt.lib`

---

### 1.3 复制 DLL 文件

**关键步骤**：Assimp 需要运行时 DLL

#### 方法 1：手动复制
```bash
# 复制 Debug DLL
copy "8502_CrouseWork\Dependencies\Assimp\bin\x64\Debug\assimp-vc143-mtd.dll" "x64\Debug\"

# 复制 Release DLL
copy "8502_CrouseWork\Dependencies\Assimp\bin\x64\Release\assimp-vc143-mt.dll" "x64\Release\"
```

#### 方法 2：自动复制（推荐）

在 `.vcxproj` 中添加后期生成事件：

```xml
<PostBuildEvent>
  <Command>
    xcopy /Y /D "$(ProjectDir)Dependencies\Assimp\bin\x64\$(Configuration)\*.dll" "$(OutDir)"
  </Command>
  <Message>复制 Assimp DLL 到输出目录</Message>
</PostBuildEvent>
```

---

## 第二步：修改代码

### 2.1 对比：数据结构差异

#### tiny_obj_loader
```cpp
tinyobj::attrib_t attrib;               // 所有顶点属性（扁平数组）
std::vector<tinyobj::shape_t> shapes;   // 形状列表
std::vector<tinyobj::material_t> materials;  // 材质列表
```

#### Assimp
```cpp
const aiScene* scene;          // 场景对象（包含所有数据）
aiNode* node;                  // 场景图节点（层级结构）
aiMesh* mesh;                  // 网格对象（包含顶点/面）
aiMaterial* material;          // 材质对象
```

**核心差异**：
- tiny_obj: 扁平的数组结构，需手动组装
- Assimp: 层级化的场景图，已经组织好的网格

---

### 2.2 修改 `Model.h`

#### 改动点
1. 移除 `tiny_obj_loader.h` 头文件
2. 添加 Assimp 头文件
3. 修改 `LoadOBJ` 为 `LoadModel`（支持多格式）
4. 添加递归处理节点的函数

#### 新的 `Model.h`
```cpp
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

class Model
{
public:
    Model(const std::string& filePath);
    ~Model();

    void Render();

    bool IsLoaded() const { return m_Loaded; }
    size_t GetMeshCount() const { return m_Meshes.size(); }

private:
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    bool m_Loaded;
    std::string m_Directory;

    // 新增：Assimp 加载函数
    bool LoadModel(const std::string& filePath);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::shared_ptr<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type);
};

#endif // MODEL_H
```

---

### 2.3 修改 `Model.cpp`

#### 完整实现
```cpp
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

    // 提取目录路径
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos)
    {
        m_Directory = filePath.substr(0, lastSlash + 1);
    }

    // 加载模型
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

Model::~Model()
{
    std::cout << "[Model] 模型已销毁" << std::endl;
}

// ========================================
// 加载模型（支持多种格式）
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
    // aiProcess_GenNormals: 如果没有法向量，自动生成
    // aiProcess_CalcTangentSpace: 计算切线和副切线（用于法线贴图）
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
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
    for (auto& mesh : m_Meshes)
    {
        mesh->Render();
    }
}
```

---

## 第三步：测试和验证

### 3.1 测试 OBJ 模型

确保现有的 OBJ 模型仍然正常工作：

```cpp
// 在 main.cpp 中
Model treeModel("Models/tree.obj");
```

### 3.2 测试新格式（FBX）

```cpp
// 测试 FBX 模型
Model characterModel("Models/character.fbx");
```

### 3.3 支持的格式列表

Assimp 支持以下格式（部分）：

| 格式 | 扩展名 | 常用场景 |
|------|--------|----------|
| **OBJ** | .obj | 静态模型 |
| **FBX** | .fbx | 动画模型、游戏资产 |
| **glTF** | .gltf, .glb | Web3D、现代引擎 |
| **Collada** | .dae | 工具互操作 |
| **Blender** | .blend | 直接加载 Blender 文件 |
| **3DS** | .3ds | 3DS Max 模型 |
| **STL** | .stl | 3D 打印 |

---

## 第四步：高级功能

### 4.1 启用更多后处理选项

```cpp
const aiScene* scene = importer.ReadFile(filePath,
    aiProcess_Triangulate |           // 三角化
    aiProcess_FlipUVs |               // 翻转 UV
    aiProcess_GenNormals |            // 生成法向量
    aiProcess_GenSmoothNormals |      // 生成平滑法向量（替代 GenNormals）
    aiProcess_CalcTangentSpace |      // 计算切线（法线贴图）
    aiProcess_JoinIdenticalVertices | // 合并重复顶点
    aiProcess_OptimizeMeshes |        // 优化网格
    aiProcess_SortByPType             // 按图元类型排序
);
```

### 4.2 加载多种纹理类型

```cpp
// 漫反射纹理
auto diffuseTexture = LoadMaterialTexture(material, aiTextureType_DIFFUSE);

// 法线贴图
auto normalTexture = LoadMaterialTexture(material, aiTextureType_NORMALS);

// 镜面反射贴图
auto specularTexture = LoadMaterialTexture(material, aiTextureType_SPECULAR);

// 高度图（视差映射）
auto heightTexture = LoadMaterialTexture(material, aiTextureType_HEIGHT);
```

### 4.3 支持骨骼动画

Assimp 提供 `aiAnimation` 和 `aiBone` 结构，可用于实现骨骼动画系统。

---

## 第五步：清理旧代码

### 5.1 移除 tiny_obj_loader

1. 删除 `Dependencies/TinyOBJ/` 目录
2. 从项目中移除相关头文件引用
3. 清理 `.vcxproj` 和 `.vcxproj.filters` 中的引用

### 5.2 更新文档

更新以下文档：
- `OBJ模型加载-快速开始.md` → `模型加载-快速开始.md`（更新为 Assimp）
- `CLAUDE.md` 中更新依赖列表

---

## 常见问题

### Q1: DLL 找不到错误
**错误**：`找不到 assimp-vc143-mtd.dll`

**解决方案**：
- 确认 DLL 已复制到 `x64/Debug/` 或 `x64/Release/` 目录
- 检查 DLL 版本是否匹配（vc143 = VS2022）

---

### Q2: 链接错误 LNK2019
**错误**：`无法解析的外部符号`

**解决方案**：
- 确认已添加 `assimp-vc143-mt.lib` 到附加依赖项
- 确认库目录路径正确
- 确认 Configuration（Debug/Release）与库版本匹配

---

### Q3: 模型加载后是黑色的
**原因**：法向量问题或光照未设置

**解决方案**：
- 确保启用了 `aiProcess_GenNormals` 或 `aiProcess_GenSmoothNormals`
- 检查着色器是否正确计算光照
- 在 Render 前打印法向量值调试

---

### Q4: 纹理路径错误
**原因**：材质文件中的路径可能是绝对路径或使用反斜杠

**解决方案**：
```cpp
// 规范化路径
std::string texturePath = path.C_Str();
std::replace(texturePath.begin(), texturePath.end(), '\\', '/');

// 仅保留文件名
size_t lastSlash = texturePath.find_last_of("/\\");
if (lastSlash != std::string::npos)
{
    texturePath = texturePath.substr(lastSlash + 1);
}

texturePath = m_Directory + texturePath;
```

---

## 总结

### 迁移清单

- [ ] 下载 Assimp SDK
- [ ] 配置项目包含目录和库目录
- [ ] 添加 DLL 复制到输出目录的后期生成事件
- [ ] 修改 `Model.h`（添加 Assimp 头文件和新函数）
- [ ] 重写 `Model.cpp`（实现 Assimp 加载逻辑）
- [ ] 测试现有 OBJ 模型
- [ ] 测试新格式（FBX/GLTF）
- [ ] 移除 tiny_obj_loader 相关代码
- [ ] 更新文档

### 优势总结

- ✅ 支持 40+ 种模型格式
- ✅ 自动生成法向量和切线
- ✅ 更好的材质和纹理支持
- ✅ 内置场景图结构
- ✅ 支持动画系统
- ✅ 活跃的社区维护

### 后续建议

1. **实现骨骼动画**：利用 Assimp 的 `aiAnimation` 和 `aiBone` 结构
2. **PBR 材质**：支持 Metallic-Roughness 工作流
3. **多纹理类型**：法线贴图、高度图、AO 贴图等
4. **场景图渲染**：利用 Assimp 的节点层级优化渲染

---

## 参考资源

- [Assimp 官方文档](https://assimp.sourceforge.net/lib_html/index.html)
- [LearnOpenGL - Model Loading](https://learnopengl-cn.github.io/03%20Model%20Loading/01%20Assimp/)
- [Assimp GitHub](https://github.com/assimp/assimp)
