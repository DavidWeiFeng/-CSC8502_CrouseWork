# OBJ模型加载系统 - 快速开始指南

## ✅ 已完成的工作

恭喜！OBJ模型加载系统已经完全实现并集成到你的项目中了！

### 创建的文件

1. **核心类文件**
   - `8502_CrouseWork/Mesh.h` - 网格类头文件
   - `8502_CrouseWork/Mesh.cpp` - 网格类实现
   - `8502_CrouseWork/Model.h` - 模型类头文件
   - `8502_CrouseWork/Model.cpp` - 模型类实现

2. **测试文件**
   - `8502_CrouseWork/Models/cube.obj` - 测试用立方体模型

3. **文档**
   - `OBJ模型加载完整实现指南.md` - 详细实现文档
   - `模型加载测试指南.md` - 测试指南
   - `OBJ模型加载-快速开始.md` - 本文档

### 项目配置更新

- ✅ `.vcxproj` - 已添加Mesh和Model类
- ✅ `.vcxproj.filters` - 已配置文件分类
- ✅ `main.cpp` - 已集成模型加载和渲染代码

---

## 🚀 立即测试

### 步骤1：编译项目

打开 Visual Studio，按 **F7** 编译项目。

**预期结果：**
```
Build succeeded.
0 Warning(s)
0 Error(s)
```

### 步骤2：运行程序

按 **F5** 运行程序（Debug模式）

### 步骤3：查看控制台输出

你应该看到类似的输出：

```
正在测试模型加载...

========================================
开始加载模型: Models/cube.obj
========================================
[LoadOBJ] 文件解析成功
  - 顶点数: 8
  - 法向量数: 6
  - 纹理坐标数: 4
  - 形状数: 1

[LoadOBJ] 处理形状 1 ()
  - 顶点: 36, 三角形: 12
[Mesh] 创建网格：36 顶点, 12 三角形
  - VAO: 7, VBO: 8, EBO: 9
✓ 模型加载成功！
  - 网格数量: 1
========================================

✓ 测试模型加载成功！
  提示：你可以在场景中看到一个旋转的立方体
```

### 步骤4：在场景中观察

运行程序后，你应该能看到：
- ✅ 地形
- ✅ 天空盒
- ✅ 水面
- ✅ **一个旋转的立方体**（在相机前方）

立方体会自动旋转，位于相机前方30米处，高度10米。

---

## 📝 如果模型没有加载

如果控制台输出：

```
ℹ 模型文件不存在，跳过模型加载
```

**解决方法：**

1. **检查Models文件夹**
   ```bash
   # 应该存在这个文件
   8502_CrouseWork\Models\cube.obj
   ```

2. **检查工作目录**
   - 右键项目 → 属性 → 调试 → 工作目录
   - 应该设置为：`$(ProjectDir)..`

3. **使用绝对路径测试**
   修改main.cpp第299行：
   ```cpp
   testModel = std::make_unique<Model>("D:\\UGit\\-CSC8502_CrouseWork\\8502_CrouseWork\\Models\\cube.obj");
   ```

---

## 🎯 下一步：加载真实模型

### 1. 下载模型

推荐网站：
- **Sketchfab**: https://sketchfab.com/
  - 搜索 "tree low poly"
  - 筛选 CC License（免费）
  - 下载格式选择 **OBJ**

- **Free3D**: https://free3d.com/
  - 搜索 "tree" 或 "rock"
  - 下载 OBJ 格式

### 2. 放置模型

将下载的模型文件（.obj、.mtl、纹理图片）放到：
```
8502_CrouseWork\Models\tree.obj
```

### 3. 修改代码加载新模型

在main.cpp第299行，修改文件路径：

```cpp
// 从：
testModel = std::make_unique<Model>("Models/cube.obj");

// 改为：
testModel = std::make_unique<Model>("Models/tree.obj");
```

### 4. 调整位置和缩放

在main.cpp第439-448行，根据模型大小调整：

```cpp
// 位置：根据需要调整
modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.0f, -50.0f));

// 缩放：模型太小就放大，太大就缩小
modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));  // 调整这个值
```

---

## 🌲 批量放置多个模型

### 方法1：手动放置3-5个树

在main.cpp的渲染循环中，复制模型渲染代码：

```cpp
// 树木1
{
    glm::mat4 tree1Matrix = glm::mat4(1.0f);
    tree1Matrix = glm::translate(tree1Matrix, glm::vec3(10.0f, 0.0f, 20.0f));
    tree1Matrix = glm::scale(tree1Matrix, glm::vec3(2.0f));
    terrainShader.SetMat4("model", tree1Matrix);
    treeModel->Render();
}

// 树木2
{
    glm::mat4 tree2Matrix = glm::mat4(1.0f);
    tree2Matrix = glm::translate(tree2Matrix, glm::vec3(-15.0f, 0.0f, 30.0f));
    tree2Matrix = glm::rotate(tree2Matrix, glm::radians(45.0f), glm::vec3(0,1,0));
    tree2Matrix = glm::scale(tree2Matrix, glm::vec3(1.8f));
    terrainShader.SetMat4("model", tree2Matrix);
    treeModel->Render();
}

// ... 继续添加更多树木
```

### 方法2：使用循环批量放置（推荐）

创建一个辅助函数（在main.cpp中，main函数之前）：

```cpp
// ========================================
// 辅助函数：在地形上放置多个树木
// ========================================
void RenderTreesOnTerrain(Model* treeModel,
                          const Terrain& terrain,
                          Shader& shader,
                          int numTrees)
{
    // 预定义的树木位置（手动调整获得最佳效果）
    std::vector<glm::vec3> treePositions = {
        glm::vec3(10.0f, 0.0f, 20.0f),
        glm::vec3(-15.0f, 0.0f, 30.0f),
        glm::vec3(25.0f, 0.0f, -10.0f),
        glm::vec3(-30.0f, 0.0f, -20.0f),
        glm::vec3(5.0f, 0.0f, -35.0f),
        glm::vec3(18.0f, 0.0f, 15.0f),
        glm::vec3(-8.0f, 0.0f, 25.0f),
        glm::vec3(22.0f, 0.0f, -25.0f)
    };

    for (int i = 0; i < std::min(numTrees, (int)treePositions.size()); ++i)
    {
        glm::vec3 pos = treePositions[i];

        // 获取该位置的地形高度
        float terrainHeight = terrain.GetHeightAt(pos.x, pos.z);

        // 构建模型矩阵
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, terrainHeight, pos.z));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(i * 45.0f), glm::vec3(0,1,0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));

        shader.SetMat4("model", modelMatrix);
        treeModel->Render();
    }
}
```

然后在渲染循环中使用：

```cpp
// 在渲染循环中
if (treeModel != nullptr && treeModel->IsLoaded())
{
    RenderTreesOnTerrain(treeModel.get(), terrain, terrainShader, 8);
}
```

---

## 🐛 常见问题

### Q1: 编译错误

**错误：** `cannot open include file 'tiny_obj_loader.h'`

**解决：**
- 确认 `Dependencies\TinyOBJ\tiny_obj_loader.h` 存在
- 检查项目属性 → C/C++ → 附加包含目录

### Q2: 模型不显示

**可能原因：**
1. 模型太小 → 增加缩放系数
2. 模型在相机后面 → 调整位置
3. 模型是黑色的 → 提高环境光

**解决：**
```cpp
// 1. 放大模型
modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f));

// 2. 确保在相机前方
modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 5, -20));

// 3. 提高环境光（在设置光照参数处）
terrainShader.SetFloat("ambientStrength", 0.5f);
```

### Q3: 模型是白色/纯色

**原因：** 模型没有纹理，或者材质文件(.mtl)未加载

**说明：** 当前实现还不支持材质文件，模型会使用纯色渲染（由光照系统提供颜色）

**解决：** 这是正常的，你可以：
- 使用有颜色的模型
- 或者等待后续添加纹理支持功能

---

## 📚 详细文档

如果需要更详细的说明，请查看：

- **`OBJ模型加载完整实现指南.md`** - 完整的实现文档
- **`模型加载测试指南.md`** - 详细的测试步骤
- **代码注释** - 所有代码都有详细的中文注释

---

## 🎉 完成！

恭喜你完成了OBJ模型加载系统的实现！

**你现在可以：**
- ✅ 加载任何OBJ格式的3D模型
- ✅ 在场景中放置、旋转、缩放模型
- ✅ 批量生成森林场景
- ✅ 为后续的场景图系统打好基础

**下一步建议：**
1. 下载几个树木和岩石模型
2. 在场景中放置20-30个模型形成森林
3. 实现场景图系统统一管理
4. 添加自动相机路径展示场景

有任何问题，随时参考详细文档或询问！😊
