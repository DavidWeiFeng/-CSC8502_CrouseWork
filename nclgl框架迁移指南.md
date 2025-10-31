# nclgl 框架迁移指南

## 📋 目录
1. [迁移概述](#迁移概述)
2. [框架对比分析](#框架对比分析)
3. [第三方库变更](#第三方库变更)
4. [迁移步骤](#迁移步骤)
5. [详细迁移任务](#详细迁移任务)
6. [关键代码修改示例](#关键代码修改示例)
7. [预期问题和解决方案](#预期问题和解决方案)
8. [测试验证计划](#测试验证计划)

---

## 迁移概述

### 当前状态
- 使用自定义的 Window/Shader/Camera 类
- 依赖 GLFW、GLM、STB、Assimp 等第三方库
- 独立的渲染循环实现

### 目标状态
- 使用老师提供的 nclgl 框架
- 仅使用 Third Party 文件夹中的库（GLAD、SOIL）
- 继承 OGLRenderer 基类实现渲染
- 使用 nclgl 的数学库（Matrix4, Vector3 等）
- 使用 nclgl 的 Window 和 Shader 系统

### 迁移规模评估
**复杂度：高**
- 涉及核心架构重构
- 需要替换所有数学库调用（GLM → nclgl）
- 需要重写窗口和输入系统（GLFW → Win32/nclgl）
- 需要转换纹理加载（STB → SOIL）
- 需要转换模型加载（Assimp/TinyOBJ → .msh 格式）

**估计工作量：** 6-10 小时

---

## 框架对比分析

### 1. 窗口系统

| 功能 | 当前实现 | nclgl 框架 |
|------|---------|-----------|
| **基础库** | GLFW | Win32 API |
| **窗口类** | `Window` (GLFW wrapper) | `Window` (Win32 wrapper) |
| **初始化** | `Window::Initialize()` | 自动初始化 |
| **事件循环** | `glfwPollEvents()` | `Window::UpdateWindow()` |
| **关闭检查** | `window.ShouldClose()` | `!window.UpdateWindow()` |
| **缓冲交换** | `window.SwapBuffers()` | 在 `UpdateWindow()` 中自动处理 |

### 2. 数学库

| 类型 | 当前实现 (GLM) | nclgl 框架 |
|------|---------------|-----------|
| **三维向量** | `glm::vec3` | `Vector3` |
| **四维向量** | `glm::vec4` | `Vector4` |
| **二维向量** | `glm::vec2` | `Vector2` |
| **矩阵** | `glm::mat4` | `Matrix4` |
| **四元数** | `glm::quat` | `Quaternion` |
| **矩阵变换** | `glm::translate()`, `glm::rotate()` | `Matrix4::Translation()`, `Matrix4::Rotation()` |
| **投影矩阵** | `glm::perspective()` | `Matrix4::Perspective()` |
| **视图矩阵** | `glm::lookAt()` | `Matrix4::BuildViewMatrix()` |

### 3. 着色器系统

| 功能 | 当前实现 | nclgl 框架 |
|------|---------|-----------|
| **构造函数** | `Shader(vertPath, fragPath)` | `Shader(vertPath, fragPath, geomPath, ...)` |
| **激活** | `shader.Use()` | 通过 `OGLRenderer::BindShader(&shader)` |
| **统一变量设置** | `shader.SetMat4(name, mat)` | 直接使用 `glUniformXX()` |
| **程序ID** | `shader.GetProgram()` | `shader.GetProgram()` |

### 4. 网格系统

| 功能 | 当前实现 | nclgl 框架 |
|------|---------|-----------|
| **格式** | 自定义 + OBJ (Assimp) | `.msh` (专有格式) |
| **加载** | `Model::LoadModel()` | `Mesh::LoadFromMeshFile()` |
| **渲染** | 自定义 VAO/VBO | `mesh->Draw()` |
| **动画支持** | 通过 Assimp | `MeshAnimation` 类 |
| **材质支持** | 通过 Assimp | `MeshMaterial` 类 |

### 5. 纹理系统

| 功能 | 当前实现 (STB) | nclgl 框架 (SOIL) |
|------|---------------|------------------|
| **加载函数** | `stbi_load()` | `SOIL_load_OGL_texture()` |
| **立方体贴图** | `stbi_load()` × 6 | `SOIL_load_OGL_cubemap()` |
| **格式支持** | PNG, JPG, TGA, BMP | PNG, JPG, TGA, BMP, DDS |
| **返回值** | 原始像素数据 | 直接返回 OpenGL 纹理 ID |

### 6. 输入系统

| 功能 | 当前实现 (GLFW) | nclgl 框架 |
|------|----------------|-----------|
| **键盘检测** | `glfwGetKey()` | `Keyboard::KeyDown()`, `KeyPressed()` |
| **鼠标位置** | `glfwGetCursorPos()` | `Mouse::GetAbsolutePosition()` |
| **鼠标按钮** | `glfwGetMouseButton()` | `Mouse::ButtonDown()` |
| **鼠标滚轮** | GLFW 回调 | `Mouse::GetWheelMovement()` |
| **回调机制** | 需要手动设置回调 | 内置在 `Window::UpdateWindow()` |

---

## 第三方库变更

### 需要移除的库（从 Dependencies 文件夹）
1. **GLFW** - 窗口管理（替换为 nclgl Window）
2. **GLM** - 数学库（替换为 nclgl Matrix/Vector）
3. **STB** - 图像加载（替换为 SOIL）
4. **Assimp** - 模型加载（替换为 .msh 格式）
5. **TinyOBJ** - OBJ 加载（不再需要）

### 保留的库（迁移到 Third Party）
1. **GLAD** - OpenGL 函数加载器（已在 Third Party 中）

### 新增的库（Third Party 中提供）
1. **SOIL** - Simple OpenGL Image Library（纹理加载）
2. **KHR** - OpenGL 平台相关头文件

---

## 迁移步骤

### 阶段 1：项目配置准备 ⏱️ 30 分钟

#### 1.1 备份当前项目
```bash
# 创建 git 分支备份
git checkout -b backup-before-nclgl-migration
git add .
git commit -m "Backup before nclgl framework migration"
git checkout master
```

#### 1.2 创建 nclgl 静态库项目
- 确保 nclgl.vcxproj 在解决方案中
- 配置为静态库 (.lib)
- 编译 nclgl 项目生成 nclgl.lib

#### 1.3 配置主项目依赖
修改 `8502_CrouseWork.vcxproj`：

**包含目录：**
```
$(ProjectDir)nclgl\
$(ProjectDir)Third Party\glad\
$(ProjectDir)Third Party\SOIL\
```

**库目录：**
```
$(ProjectDir)nclgl\x64\$(Configuration)\
$(ProjectDir)Third Party\SOIL\x64\
```

**附加依赖项：**
```
nclgl.lib
SOIL.lib
opengl32.lib
```

**移除旧依赖：**
- 删除 GLFW、GLM、STB、Assimp 的包含路径
- 删除 glfw3.lib、assimp 相关库的链接

---

### 阶段 2：核心架构迁移 ⏱️ 2-3 小时

#### 2.1 创建主渲染器类

**文件：** `Renderer.h` 和 `Renderer.cpp`

```cpp
// Renderer.h
#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"  // 稍后创建

class Terrain;   // 前向声明
class Skybox;
class WaterPlane;

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    virtual ~Renderer();

    virtual void RenderScene() override;
    virtual void UpdateScene(float msec) override;

protected:
    void UpdateCamera(float msec);
    void RenderTerrain();
    void RenderSkybox();
    void RenderWater();

private:
    // Camera* camera;  // 新的 Camera 类
    Terrain* terrain;
    Skybox* skybox;
    WaterPlane* water;

    Shader* terrainShader;
    Shader* skyboxShader;
    Shader* waterShader;
};
```

#### 2.2 创建新的 Camera 类

**文件：** `Camera.h` 和 `Camera.cpp`

将现有 Camera 从 GLM 迁移到 nclgl 数学类型：

```cpp
// Camera.h
#pragma once
#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"

class Camera {
public:
    Camera(Vector3 position = Vector3(0, 0, 5));

    Matrix4 BuildViewMatrix();

    void UpdateCamera(float msec);  // 处理键盘和鼠标

    Vector3 GetPosition() const { return position; }
    void SetPosition(Vector3 pos) { position = pos; }

    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }
    float GetFOV() const { return fov; }

private:
    void UpdateVectors();

    Vector3 position;
    Vector3 front;
    Vector3 up;
    Vector3 right;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float fov;
};
```

#### 2.3 重写 main.cpp

```cpp
// main.cpp
#include "../nclgl/Window.h"
#include "Renderer.h"
#include <iostream>

int main() {
    // 创建窗口（1280x720）
    Window w("CSC8502 Coursework", 1280, 720, false);

    if (!w.HasInitialised()) {
        std::cerr << "Failed to initialize window!" << std::endl;
        return -1;
    }

    // 创建渲染器
    Renderer renderer(w);

    if (!renderer.HasInitialised()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }

    w.SetRenderer(&renderer);

    // 主循环
    while (w.UpdateWindow()) {
        float msec = w.GetTimer()->GetTimeDeltaSeconds() * 1000.0f;
        renderer.UpdateScene(msec);
        renderer.RenderScene();
    }

    return 0;
}
```

---

### 阶段 3：数学库迁移 ⏱️ 2-3 小时

#### 3.1 替换规则

创建一个"查找替换"清单：

| GLM 代码 | nclgl 代码 |
|---------|-----------|
| `glm::vec3(x, y, z)` | `Vector3(x, y, z)` |
| `glm::vec4(x, y, z, w)` | `Vector4(x, y, z, w)` |
| `glm::vec2(x, y)` | `Vector2(x, y)` |
| `glm::mat4(1.0f)` | `Matrix4()` (已是单位矩阵) |
| `glm::translate(mat, vec)` | `Matrix4::Translation(vec) * mat` |
| `glm::rotate(mat, angle, axis)` | `Matrix4::Rotation(angle, axis) * mat` |
| `glm::scale(mat, vec)` | `Matrix4::Scale(vec) * mat` |
| `glm::perspective(fov, aspect, near, far)` | `Matrix4::Perspective(near, far, aspect, fov)` |
| `glm::lookAt(pos, target, up)` | `Matrix4::BuildViewMatrix(pos, target, up)` |
| `glm::value_ptr(mat)` | `mat.values` (直接访问数组) |

#### 3.2 需要修改的文件列表

修改所有使用 GLM 的文件：
- ✅ `Camera.h` / `Camera.cpp`
- ✅ `Terrain.h` / `Terrain.cpp`
- ✅ `Skybox.h` / `Skybox.cpp`
- ✅ `WaterPlane.h` / `WaterPlane.cpp`
- ✅ `Model.h` / `Model.cpp` (或删除，改用 Mesh)
- ✅ `Mesh.h` / `Mesh.cpp` (合并到 nclgl Mesh)
- ✅ `main.cpp`
- ✅ 所有着色器 uniform 设置代码

#### 3.3 注意事项

**矩阵布局差异：**
- GLM 默认列主序
- nclgl Matrix4 也是列主序
- **但 nclgl 直接暴露 `float values[16]` 数组**

**向量访问：**
```cpp
// GLM
vec.x, vec.y, vec.z

// nclgl (相同)
vec.x, vec.y, vec.z
```

**数学运算：**
```cpp
// GLM (运算符重载丰富)
vec3 result = vec1 + vec2 * 2.0f;

// nclgl (需要检查是否支持所有运算符)
Vector3 result = vec1 + vec2 * 2.0f;  // 需要确认 * 是否重载
```

---

### 阶段 4：纹理系统迁移 ⏱️ 1 小时

#### 4.1 使用 SOIL 替换 STB

**旧代码 (STB)：**
```cpp
// Texture.cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint textureID;
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);

int width, height, channels;
unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

if (data) {
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
stbi_image_free(data);
```

**新代码 (SOIL)：**
```cpp
// 使用 SOIL - 更简单！
#include "SOIL/SOIL.h"

GLuint textureID = SOIL_load_OGL_texture(
    path.c_str(),
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
);

if (textureID == 0) {
    std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
}
```

#### 4.2 立方体贴图（天空盒）

**旧代码 (STB)：**
```cpp
GLuint cubemapID;
glGenTextures(1, &cubemapID);
glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

for (int i = 0; i < 6; i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ...);
    stbi_image_free(data);
}
```

**新代码 (SOIL)：**
```cpp
GLuint cubemapID = SOIL_load_OGL_cubemap(
    faces[0].c_str(),  // right
    faces[1].c_str(),  // left
    faces[2].c_str(),  // top
    faces[3].c_str(),  // bottom
    faces[4].c_str(),  // front
    faces[5].c_str(),  // back
    SOIL_LOAD_RGB,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_MIPMAPS
);
```

#### 4.3 修改 Texture 类

**选项 1：** 保留自定义 Texture 类，内部使用 SOIL
**选项 2：** 直接在需要的地方使用 SOIL，删除 Texture 类

**推荐选项 1** - 保持代码结构：

```cpp
// Texture.h
#pragma once
#include "../nclgl/OGLRenderer.h"

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    void Bind(unsigned int slot = 0);
    GLuint GetID() const { return textureID; }

private:
    GLuint textureID;
};
```

---

### 阶段 5：模型加载迁移 ⏱️ 1-2 小时

#### 5.1 转换模型格式

**当前状态：**
- 使用 Assimp 加载 .obj 文件

**目标状态：**
- 使用 nclgl 的 `Mesh::LoadFromMeshFile()` 加载 .msh 文件

**问题：** `.msh` 是专有格式，需要转换工具

**解决方案：**

1. **查找转换工具：**
   - 检查 nclgl 文件夹是否有工具（MeshConverter.exe 等）
   - 询问老师/同学是否有转换工具
   - 使用课程提供的 Meshes 文件夹中的现有模型

2. **使用现有 .msh 文件：**
   ```
   Meshes/Cube.msh
   Meshes/Sphere.msh
   Meshes/Cone.msh
   Meshes/Role_T.msh  (带动画的人物模型)
   ```

3. **简化方案 - 使用基础形状：**
   ```cpp
   // 加载立方体用作树木的替代
   Mesh* treeMesh = Mesh::LoadFromMeshFile("Meshes/Cube.msh");

   // 加载球体用作岩石的替代
   Mesh* rockMesh = Mesh::LoadFromMeshFile("Meshes/Sphere.msh");
   ```

#### 5.2 修改场景对象加载

**删除或注释掉 Model 类相关代码：**

```cpp
// 旧代码
// #include "Model.h"
// Model* tree = new Model("Models/tree.obj");

// 新代码
#include "../nclgl/Mesh.h"
Mesh* tree = Mesh::LoadFromMeshFile("Meshes/Cube.msh");  // 临时替代
```

**如果必须使用 OBJ 模型：**
- 保留现有的 Mesh.cpp 中的 OBJ 加载代码
- 但移除 Assimp 依赖，使用手动解析或 TinyOBJ（如果允许）

---

### 阶段 6：Shader 系统适配 ⏱️ 30 分钟

#### 6.1 Shader 加载

nclgl Shader 已经提供了完整功能，**直接使用即可**。

**变更：**

```cpp
// 旧代码
Shader* shader = new Shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");
shader->Use();
shader->SetMat4("model", modelMatrix);

// 新代码
Shader* shader = new Shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");
BindShader(shader);  // 在 OGLRenderer 中使用
glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "model"),
                   1, false, (float*)&modelMatrix.values);
```

#### 6.2 Uniform 设置助手函数

nclgl Shader 没有提供 `SetMat4()` 等便利函数，需要手动设置：

**在 Renderer 类中添加辅助函数：**

```cpp
// Renderer.h
protected:
    void SetShaderMatrix4(const char* name, const Matrix4& mat);
    void SetShaderVector3(const char* name, const Vector3& vec);
    void SetShaderFloat(const char* name, float value);
    void SetShaderInt(const char* name, int value);
```

```cpp
// Renderer.cpp
void Renderer::SetShaderMatrix4(const char* name, const Matrix4& mat) {
    GLuint loc = glGetUniformLocation(currentShader->GetProgram(), name);
    glUniformMatrix4fv(loc, 1, false, mat.values);
}

void Renderer::SetShaderVector3(const char* name, const Vector3& vec) {
    GLuint loc = glGetUniformLocation(currentShader->GetProgram(), name);
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Renderer::SetShaderFloat(const char* name, float value) {
    GLuint loc = glGetUniformLocation(currentShader->GetProgram(), name);
    glUniform1f(loc, value);
}

void Renderer::SetShaderInt(const char* name, int value) {
    GLuint loc = glGetUniformLocation(currentShader->GetProgram(), name);
    glUniform1i(loc, value);
}
```

---

### 阶段 7：输入系统迁移 ⏱️ 30 分钟

#### 7.1 键盘输入

**旧代码 (GLFW)：**
```cpp
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
}
```

**新代码 (nclgl)：**
```cpp
void Renderer::UpdateCamera(float msec) {
    Keyboard* kb = Window::GetKeyboard();

    if (kb->KeyDown(KEYBOARD_W)) {
        camera->ProcessKeyboard(FORWARD, msec / 1000.0f);
    }
    if (kb->KeyDown(KEYBOARD_S)) {
        camera->ProcessKeyboard(BACKWARD, msec / 1000.0f);
    }
    // ... 其他按键
}
```

**键码映射：**
```cpp
// GLFW → nclgl
GLFW_KEY_W       → KEYBOARD_W
GLFW_KEY_A       → KEYBOARD_A
GLFW_KEY_S       → KEYBOARD_S
GLFW_KEY_D       → KEYBOARD_D
GLFW_KEY_SPACE   → KEYBOARD_SPACE
GLFW_KEY_LEFT_SHIFT → KEYBOARD_SHIFT
GLFW_KEY_ESCAPE  → KEYBOARD_ESCAPE
```

#### 7.2 鼠标输入

**旧代码 (GLFW 回调)：**
```cpp
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}
```

**新代码 (nclgl)：**
```cpp
void Renderer::UpdateCamera(float msec) {
    Mouse* mouse = Window::GetMouse();

    Vector2 mousePos = mouse->GetRelativePosition();
    camera->ProcessMouseMovement(mousePos.x, -mousePos.y);

    // 鼠标滚轮
    int wheelDelta = mouse->GetWheelMovement();
    if (wheelDelta != 0) {
        camera->ProcessMouseScroll((float)wheelDelta);
    }
}
```

---

## 详细迁移任务

### 任务清单

#### ✅ 阶段 1：环境配置
- [ ] 1.1 创建备份分支
- [ ] 1.2 编译 nclgl 静态库
- [ ] 1.3 修改项目包含路径
- [ ] 1.4 修改项目库路径
- [ ] 1.5 移除旧依赖库链接
- [ ] 1.6 添加 nclgl.lib 和 SOIL.lib 链接
- [ ] 1.7 测试编译 nclgl 框架

#### ✅ 阶段 2：核心架构
- [ ] 2.1 创建 `Renderer.h` / `Renderer.cpp`
- [ ] 2.2 实现 `RenderScene()` 和 `UpdateScene()`
- [ ] 2.3 重写 `Camera.h` / `Camera.cpp`
- [ ] 2.4 修改 `main.cpp` 使用新架构
- [ ] 2.5 测试编译 - 预期大量错误

#### ✅ 阶段 3：数学库迁移
- [ ] 3.1 全局查找替换 `glm::vec3` → `Vector3`
- [ ] 3.2 全局查找替换 `glm::vec4` → `Vector4`
- [ ] 3.3 全局查找替换 `glm::mat4` → `Matrix4`
- [ ] 3.4 修改所有矩阵变换函数调用
- [ ] 3.5 修改投影和视图矩阵创建
- [ ] 3.6 移除所有 `#include <glm/...>`
- [ ] 3.7 添加 `#include` nclgl 数学类
- [ ] 3.8 修复编译错误

#### ✅ 阶段 4：纹理系统
- [ ] 4.1 修改 `Texture.cpp` 使用 SOIL
- [ ] 4.2 修改 `Skybox.cpp` 立方体贴图加载
- [ ] 4.3 测试纹理加载
- [ ] 4.4 验证 Y 轴翻转（SOIL_FLAG_INVERT_Y）

#### ✅ 阶段 5：模型加载
- [ ] 5.1 检查 Meshes 文件夹中的可用模型
- [ ] 5.2 使用 .msh 替换 OBJ 模型
- [ ] 5.3 或保留简化的 OBJ 加载器（无 Assimp）
- [ ] 5.4 测试模型渲染

#### ✅ 阶段 6：Shader 适配
- [ ] 6.1 修改 Shader uniform 设置
- [ ] 6.2 实现 SetShader* 辅助函数
- [ ] 6.3 使用 `BindShader()` 替换 `shader->Use()`
- [ ] 6.4 验证 `UpdateShaderMatrices()`

#### ✅ 阶段 7：输入系统
- [ ] 7.1 移除 GLFW 回调函数
- [ ] 7.2 在 `UpdateCamera()` 中使用 nclgl 输入
- [ ] 7.3 映射所有键码
- [ ] 7.4 实现鼠标相对移动
- [ ] 7.5 测试相机控制

#### ✅ 阶段 8：场景对象迁移
- [ ] 8.1 迁移 `Terrain` 类
- [ ] 8.2 迁移 `Skybox` 类
- [ ] 8.3 迁移 `WaterPlane` 类
- [ ] 8.4 迁移所有自定义对象

#### ✅ 阶段 9：测试验证
- [ ] 9.1 地形渲染测试
- [ ] 9.2 天空盒渲染测试
- [ ] 9.3 水面渲染测试
- [ ] 9.4 光照测试
- [ ] 9.5 相机控制测试
- [ ] 9.6 性能测试

---

## 关键代码修改示例

### 示例 1：Terrain 类迁移

**旧 Terrain.h (使用 GLM)：**
```cpp
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Terrain {
public:
    Terrain(const std::string& heightmapPath);
    void Draw();

private:
    glm::vec3 GetNormal(int x, int z);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    // ...
};
```

**新 Terrain.h (使用 nclgl)：**
```cpp
#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Mesh.h"
#include <vector>

class Terrain {
public:
    Terrain(const std::string& heightmapPath);
    void Draw();

private:
    Vector3 GetNormal(int x, int z);

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    // ...
};
```

### 示例 2：Camera 类迁移

**旧 Camera.cpp (GLM)：**
```cpp
glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
}
```

**新 Camera.cpp (nclgl)：**
```cpp
Matrix4 Camera::BuildViewMatrix() {
    return Matrix4::BuildViewMatrix(position, position + front, up);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position = position + front * velocity;
}
```

### 示例 3：Renderer::RenderScene()

```cpp
void Renderer::RenderScene() {
    // 清除缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 更新矩阵
    viewMatrix = camera->BuildViewMatrix();
    projMatrix = Matrix4::Perspective(0.1f, 1000.0f,
                                      (float)width / (float)height,
                                      camera->GetFOV());

    // 渲染天空盒（关闭深度写入）
    glDepthMask(GL_FALSE);
    RenderSkybox();
    glDepthMask(GL_TRUE);

    // 渲染地形
    RenderTerrain();

    // 渲染水面
    RenderWater();

    // 交换缓冲
    SwapBuffers();
}

void Renderer::RenderTerrain() {
    BindShader(terrainShader);

    UpdateShaderMatrices();  // 设置 view 和 projection

    modelMatrix.ToIdentity();
    SetShaderMatrix4("model", modelMatrix);
    SetShaderVector3("lightPos", Vector3(100, 100, 100));
    SetShaderVector3("viewPos", camera->GetPosition());

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTextureID);
    SetShaderInt("terrainTexture", 0);

    terrain->Draw();
}
```

---

## 预期问题和解决方案

### 问题 1：nclgl 数学类缺少某些运算符

**症状：**
```cpp
Vector3 result = vec1 + vec2 * 2.0f;  // 编译错误
```

**解决：**
查看 `Vector3.h` 定义，如果没有 `operator*` 重载：
```cpp
// 临时解决 - 分步操作
Vector3 scaled = Vector3(vec2.x * 2.0f, vec2.y * 2.0f, vec2.z * 2.0f);
Vector3 result = vec1 + scaled;

// 或在 Renderer 中添加辅助函数
Vector3 Renderer::ScaleVector(const Vector3& v, float scale) {
    return Vector3(v.x * scale, v.y * scale, v.z * scale);
}
```

### 问题 2：SOIL 纹理 Y 轴翻转

**症状：**
纹理上下颠倒

**解决：**
```cpp
// 加载时使用 SOIL_FLAG_INVERT_Y
GLuint tex = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
                                   SOIL_CREATE_NEW_ID,
                                   SOIL_FLAG_INVERT_Y);

// 或在着色器中翻转
// Fragment Shader
vec2 flippedUV = vec2(TexCoords.x, 1.0 - TexCoords.y);
```

### 问题 3：没有 .msh 转换工具

**解决方案 A - 使用现有模型：**
使用 `Meshes/` 文件夹中的模型，调整场景设计以适应

**解决方案 B - 保留简化 OBJ 加载：**
手动实现简单的 OBJ 解析器（不依赖 Assimp）：
```cpp
// SimpleMeshLoader.h
class SimpleMeshLoader {
public:
    static Mesh* LoadOBJ(const std::string& path);
};
```

**解决方案 C - 询问老师：**
课程可能提供了转换工具或替代方案

### 问题 4：帧时间单位不一致

**症状：**
相机移动速度异常（太快或太慢）

**原因：**
- GLFW: `deltaTime` 单位是秒
- nclgl: `GetTimeDeltaSeconds()` 返回秒，但 `UpdateScene(msec)` 参数是毫秒

**解决：**
```cpp
// main.cpp
while (w.UpdateWindow()) {
    float msec = w.GetTimer()->GetTimeDeltaSeconds() * 1000.0f;
    renderer.UpdateScene(msec);
    renderer.RenderScene();
}

// Renderer::UpdateCamera(float msec)
camera->UpdateCamera(msec / 1000.0f);  // 转回秒
```

### 问题 5：Shader 路径问题

**症状：**
找不到着色器文件

**原因：**
nclgl Shader 可能使用不同的工作目录

**解决：**
```cpp
// 使用项目相对路径
Shader* shader = new Shader("Shaders/terrainVertex.glsl",
                            "Shaders/terrainFragment.glsl");

// 或绝对路径（不推荐）
// Shader* shader = new Shader("C:/Projects/.../Shaders/...", ...);
```

---

## 测试验证计划

### 测试 1：框架基础测试

**目标：** 验证 nclgl 框架正常工作

**步骤：**
1. 创建最小化 Renderer（仅清屏）
2. 运行程序，验证窗口创建
3. 测试 ESC 键退出

**预期结果：** 蓝色窗口，无错误

### 测试 2：矩阵和变换测试

**目标：** 验证数学库正确性

**步骤：**
1. 渲染一个简单的立方体（使用 Cube.msh）
2. 应用旋转、平移、缩放
3. 设置相机和投影

**预期结果：** 立方体正常显示和变换

### 测试 3：纹理加载测试

**目标：** 验证 SOIL 纹理加载

**步骤：**
1. 加载一张简单纹理
2. 应用到立方体
3. 检查纹理方向

**预期结果：** 纹理正确显示，无翻转

### 测试 4：输入系统测试

**目标：** 验证键盘和鼠标控制

**步骤：**
1. 测试 WASD 移动
2. 测试鼠标查看
3. 测试滚轮缩放

**预期结果：** 相机响应所有输入

### 测试 5：完整场景测试

**目标：** 验证地形、天空盒、水面

**步骤：**
1. 渲染地形
2. 渲染天空盒
3. 渲染水面
4. 验证光照

**预期结果：** 完整场景显示，性能正常

---

## 总结

这是一个**重大的架构迁移**，主要变更包括：

1. **核心架构**：从独立渲染循环 → 继承 OGLRenderer
2. **窗口系统**：GLFW → Win32/nclgl Window
3. **数学库**：GLM → nclgl Vector/Matrix
4. **纹理加载**：STB → SOIL
5. **模型加载**：Assimp/OBJ → .msh 格式
6. **输入系统**：GLFW callbacks → nclgl Keyboard/Mouse

### 关键优势
- ✅ 符合课程要求
- ✅ 统一的框架接口
- ✅ 减少第三方依赖

### 关键挑战
- ⚠️ 模型格式转换（.obj → .msh）
- ⚠️ 大量代码修改
- ⚠️ 数学库 API 差异
- ⚠️ 调试时间较长

### 建议
1. **逐步迁移**：按阶段进行，每阶段编译测试
2. **保留备份**：使用 git 分支
3. **简化模型**：必要时使用简单几何体替代复杂模型
4. **寻求帮助**：向老师/同学咨询 .msh 转换工具

### 时间规划
- **快速路径**（简化模型）：4-6 小时
- **完整迁移**（包含模型转换）：8-12 小时

祝迁移顺利！🚀
